#include "exec_program.hpp"

#include <util/filesystem.hpp>
#include <util/cli.hpp>

namespace komaru::translate {

ProgramExecResult ProgramExecResult::OkResult(std::string output) {
    ProgramExecResult res;
    res.output_ = std::move(output);
    return res;
}

ProgramExecResult ProgramExecResult::CompileErrorResult(int code, std::string error) {
    ProgramExecResult res;
    res.compile_code_ = code;
    res.compile_err_ = std::move(error);
    return res;
}

ProgramExecResult ProgramExecResult::RuntimeErrorResult(int code, std::string error) {
    ProgramExecResult res;
    res.exec_code_ = code;
    res.runtime_err_ = std::move(error);
    return res;
}

const std::string& ProgramExecResult::Output() const {
    return output_;
}

bool ProgramExecResult::Success() const {
    return !WasCompileError() && !WasRuntimeError();
}

bool ProgramExecResult::WasCompileError() const {
    return compile_code_ != 0;
}

bool ProgramExecResult::WasRuntimeError() const {
    return exec_code_ != 0;
}

int ProgramExecResult::CompileExitCode() const {
    return compile_code_;
}

int ProgramExecResult::ExecExitCode() const {
    return exec_code_;
}

const std::string& ProgramExecResult::RuntimeError() const {
    return runtime_err_;
}

const std::string& ProgramExecResult::CompileError() const {
    return compile_err_;
}

ProgramExecResult::ProgramExecResult() {}

ProgramExecResult ExecProgram(const IProgram& program) {
    auto sourcepath = util::GenTmpFilepath().string() + program.GetExt();
    auto progpath = util::GenTmpFilepath().string();

    if(auto err = util::WriteFile(sourcepath, program.GetSourceCode())) {
        throw std::runtime_error(std::format("failed to write file \"{}\", error \"{}\"", sourcepath, err.message()));
    }

    auto build_command = program.GetBuildCommand(sourcepath, progpath);
    auto build_result = util::PerformCLICommand(build_command);

    if(build_result.Fail()) {
        auto err = std::format("STDOUT:\n{}\nSTDERR:\n{}", build_result.Stdout(), build_result.Stderr());
        std::println("Compile error:\n{}", err);
        return ProgramExecResult::CompileErrorResult(build_result.Code(), std::move(err));
    }

    auto exec_result = util::PerformCLICommand(progpath);

    if(exec_result.Fail()) {
        auto err = std::format("STDOUT:\n{}\nSTDERR:\n{}", exec_result.Stdout(), exec_result.Stderr());
        return ProgramExecResult::RuntimeErrorResult(exec_result.Code(), std::move(err));
    }

    return ProgramExecResult::OkResult(exec_result.Stdout());
}

}
