#pragma once
#include <string>

namespace komaru::util {

class [[nodiscard("CLICommandResult must be checked for errors")]] CLICommandResult  {
public:
    CLICommandResult(int code, std::string sout, std::string serr);

    const std::string& Stdout() const;
    const std::string& Stderr() const;
    int Code() const;

    bool Success() const;
    bool Fail() const;

private:
    int code_{0};
    std::string sout_;
    std::string serr_;
};

CLICommandResult PerformCLICommand(const std::string& command);
CLICommandResult PerformCLICommand(const std::vector<std::string>& command);

}
