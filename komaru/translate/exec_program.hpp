#pragma once
#include <komaru/translate/program.hpp>

#include <string>

// TODO: This probably shouldn't be in the translate namespace
namespace komaru::translate {

class ProgramExecResult {
public:
    static ProgramExecResult OkResult(std::string output);
    static ProgramExecResult CompileErrorResult(int code, std::string error);
    static ProgramExecResult RuntimeErrorResult(int code, std::string error);

    const std::string& Output() const;
    bool Success() const;
    bool WasCompileError() const;
    bool WasRuntimeError() const;
    int CompileExitCode() const;
    int ExecExitCode() const;
    const std::string& RuntimeError() const;
    const std::string& CompileError() const;

private:
    ProgramExecResult();

private:
    std::string output_;
    std::string compile_err_;
    std::string runtime_err_;
    int compile_code_{0};
    int exec_code_{0};
};

ProgramExecResult ExecProgram(const IProgram& program, const std::string& sin = "");

}  // namespace komaru::translate
