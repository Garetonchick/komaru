#pragma once
#include <string>

namespace komaru::util {

class UnixFD {
public:
    static UnixFD FromFD(int fd);
    static UnixFD Open();

    int GetFD() const;

    std::string ReadSome();
    size_t WriteAll(const std::string& s);

private:
    explicit UnixFD(int fd, bool autoclose);
    ~UnixFD();

private:
    int fd_;
    bool autoclose_;
};

}  // namespace komaru::util
