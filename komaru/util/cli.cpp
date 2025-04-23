#include "cli.hpp"

#include <komaru/util/filesystem.hpp>
#include <komaru/util/string.hpp>

#include <sys/wait.h>
#include <unistd.h>

#include <format>

namespace komaru::util {

CLICommandResult::CLICommandResult(int code, std::string sout, std::string serr)
    : code_(code),
      sout_(std::move(sout)),
      serr_(std::move(serr)) {
}

const std::string& CLICommandResult::Stdout() const {
    return sout_;
}

const std::string& CLICommandResult::Stderr() const {
    return serr_;
}

int CLICommandResult::Code() const {
    return code_;
}

bool CLICommandResult::Success() const {
    return code_ == 0;
}

bool CLICommandResult::Fail() const {
    return code_ != 0;
}

CLICommandResult PerformCLICommand(const std::string& command, const std::string& sin) {
    auto stdin_path = GenTmpFilepath().string();
    auto stdout_path = GenTmpFilepath().string();
    auto stderr_path = GenTmpFilepath().string();

    if (!sin.empty()) {
        if (auto err = WriteFile(stdin_path, sin)) {
            throw std::runtime_error(
                std::format("failed to write stdin to file: {}", err.message()));
        }
    }

    const std::string final_command = std::invoke([&]() {
        if (sin.empty()) {
            return std::format("{} 1> {} 2> {}", command, stdout_path, stderr_path);
        }
        return std::format("{} < {} 1> {} 2> {}", command, stdin_path, stdout_path, stderr_path);
    });

    auto pid = fork();

    if (pid < 0) {
        throw std::runtime_error(
            std::format("Failed to fork when trying to execute \"{}\"", command));
    }

    if (pid == 0) {
        execlp("/bin/sh", "/bin/sh", "-c", final_command.c_str(), nullptr);
        exit(42);
    }

    int status;
    auto ret = waitpid(pid, &status, 0);
    if (ret == pid) {
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);

            auto maybe_sout = ReadFile(stdout_path);
            auto maybe_serr = ReadFile(stderr_path);
            if (!maybe_sout) {
                throw std::runtime_error(
                    std::format("error while reading stdout file \"{}\", error: {}", stdout_path,
                                maybe_sout.error().message()));
            }
            if (!maybe_serr) {
                throw std::runtime_error(
                    std::format("error while reading stdout file \"{}\", error: {}", stderr_path,
                                maybe_serr.error().message()));
            }
            std::filesystem::remove(stdout_path);
            std::filesystem::remove(stderr_path);

            return CLICommandResult(code, std::move(maybe_sout.value()),
                                    std::move(maybe_serr.value()));
        } else {
            throw std::runtime_error(std::format("Failed to execute command \"{}\"", command));
        }
    }

    kill(pid, SIGKILL);
    throw std::runtime_error(
        std::format("Failed to wait for child when executing command \"{}\"", command));
}

CLICommandResult PerformCLICommand(const std::vector<std::string>& command,
                                   const std::string& sin) {
    auto joined_command = command | JoinStrings(" ") | std::ranges::to<std::string>();

    return PerformCLICommand(joined_command, sin);
}

}  // namespace komaru::util
