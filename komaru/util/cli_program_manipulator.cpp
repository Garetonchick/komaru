#include "cli_program_manipulator.hpp"

#include <komaru/util/string.hpp>

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <format>

namespace komaru::util {

CLIProgramManipulator::CLIProgramManipulator(const std::vector<std::string>& start_command,
                                             std::string query_sep, bool skip_initial_response)
    : query_sep_(query_sep) {
    auto joined_command = start_command | JoinStrings(" ") | std::ranges::to<std::string>();

    int pipe_to[2];
    int pipe_from[2];

    pipe2(pipe_to, O_CLOEXEC);
    pipe2(pipe_from, O_CLOEXEC | O_NONBLOCK);

    write_fd_ = pipe_to[1];
    read_fd_ = pipe_from[0];

    pid_ = fork();

    if (pid_ < 0) {
        throw std::runtime_error(
            std::format("Failed to fork when trying to execute \"{}\"", joined_command));
    }

    if (pid_ == 0) {
        dup2(pipe_to[0], STDIN_FILENO);
        dup2(pipe_from[1], STDOUT_FILENO);
        dup2(pipe_from[1], STDERR_FILENO);
        execlp("/bin/sh", "/bin/sh", "-c", joined_command.c_str(), nullptr);
        exit(42);
    }

    if (skip_initial_response) {
        Receive();
    }
}

CLIProgramManipulator::~CLIProgramManipulator() {
    kill(pid_, SIGKILL);
    waitpid(pid_, nullptr, 0);
}

std::string CLIProgramManipulator::Interact(const std::string& input) {
    Send(input);
    return Receive();
}

void CLIProgramManipulator::Send(const std::string& input) {
    errno = 0;
    ssize_t n_written = write(write_fd_, input.c_str(), input.size());
    if (errno != 0) {
        throw std::runtime_error(
            std::format("failed to write to child process: {}", strerror(errno)));
    }
    if (n_written != static_cast<ssize_t>(input.size())) {
        throw std::runtime_error(
            std::format("failed to write to child process: {} bytes written, expected {} bytes",
                        n_written, input.size()));
    }
}

std::string CLIProgramManipulator::Receive() {
    std::string res;

    while (true) {
        errno = 0;
        ssize_t n_read = read(read_fd_, read_buf_, sizeof(read_buf_));
        int err = errno;
        if (n_read > 0) {
            res += std::string(read_buf_, n_read);
        }
        if (res.ends_with(query_sep_)) {
            break;
        }
        if (err == EAGAIN) {
            continue;
        }
        if (err != 0) {
            throw std::runtime_error(
                std::format("failed to read from child process: {}", strerror(err)));
        }
        if (n_read == 0) {
            break;
        }
    }

    if (res.ends_with(query_sep_)) {
        res.resize(res.size() - query_sep_.size());
    } else {
        throw std::runtime_error(
            std::format("received unexpected response from child process: \"{}\"", res));
    }
    return res;
}

}  // namespace komaru::util
