#include "exec_program.hpp"

#include <komaru/util/filesystem.hpp>

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

ProgramExecResult::ProgramExecResult() {
}

ProgramBuildResult BuildProgram(const IProgram& program, std::string progpath) {
    auto sourcepath = util::GenTmpFilepath().string() + program.GetExt();

    if (progpath.empty()) {
        progpath = util::GenTmpFilepath().string();
    }

    if (auto err = util::WriteFile(sourcepath, program.GetSourceCode())) {
        throw std::runtime_error(
            std::format("failed to write file \"{}\", error \"{}\"", sourcepath, err.message()));
    }

    auto build_command = program.GetBuildCommand(sourcepath, progpath);
    auto build_result = util::PerformCLICommand(build_command);

    return ProgramBuildResult{.command_res = build_result,
                              .program_path = build_result.Success() ? progpath : ""};
}

ProgramExecResult ExecProgram(const IProgram& program, const std::string& sin) {
    auto build_result = BuildProgram(program);

    if (build_result.command_res.Fail()) {
        auto err = std::format("STDOUT:\n{}\nSTDERR:\n{}", build_result.command_res.Stdout(),
                               build_result.command_res.Stderr());
        return ProgramExecResult::CompileErrorResult(build_result.command_res.Code(),
                                                     std::move(err));
    }

    auto exec_result = util::PerformCLICommand(build_result.program_path, sin);

    if (exec_result.Fail()) {
        auto err =
            std::format("STDOUT:\n{}\nSTDERR:\n{}", exec_result.Stdout(), exec_result.Stderr());
        return ProgramExecResult::RuntimeErrorResult(exec_result.Code(), std::move(err));
    }

    return ProgramExecResult::OkResult(exec_result.Stdout());
}

}  // namespace komaru::translate
