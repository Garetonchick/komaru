#pragma once

#include <string>
#include <vector>

#include <komaru/util/non_copyable_non_movable.hpp>

namespace komaru::util {

class CLIProgramManipulator : public NonCopyableNonMovable {
public:
    explicit CLIProgramManipulator(const std::vector<std::string>& start_command,
                                   std::string query_sep, bool skip_initial_response = true);
    ~CLIProgramManipulator();

    std::string Interact(const std::string& input);

private:
    void Send(const std::string& input);
    std::string Receive();

private:
    std::string query_sep_;
    int pid_;
    int read_fd_;
    int write_fd_;
    char read_buf_[1024];
};

}  // namespace komaru::util
