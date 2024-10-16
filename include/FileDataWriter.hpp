#ifndef FILEDATAWRITER_HPP_
#define FILEDATAWRITER_HPP_

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unistd.h>

namespace mmd {
class FileDataWriter
{
public:
    FileDataWriter(const std::string &file_path)
    {
        fd_ = ::open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_ == -1) {
            throw std::system_error(errno, std::generic_category(), "Failed to open file");
        }
    }

    ~FileDataWriter()
    {
        if (fd_ != -1) {
            ::close(fd_);
        }
    }

    void write(std::string_view data)
    {
        size_t total_written = 0;
        while (total_written < data.size()) {
            ssize_t bytes_written =
                    ::write(fd_, data.data() + total_written, data.size() - total_written);
            if (bytes_written == -1) {
                throw std::system_error(errno, std::generic_category(), "Failed to write to file");
            }
            total_written += bytes_written;
        }
    }

private:
    int fd_; // File descriptor
};
} // namespace mmd

#endif // FILEDATAWRITER_HPP_