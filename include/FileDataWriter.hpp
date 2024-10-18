/**
 * @file FileDataWriter.hpp
 * @brief Contains impelementation of FileDataWriter class.
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef FILEDATAWRITER_HPP_
#define FILEDATAWRITER_HPP_

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unistd.h>

namespace mmd {

/**
 * @brief RAII implementation of a file data writer.
 * Writes a std::string_view to a file. 
 * Throws on errors, that's way bette than errno!
 */
class FileDataWriter
{
public:
    FileDataWriter(const std::string &file_path)
    {
        _fd = ::open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (_fd == -1) {
            throw std::system_error(errno, std::generic_category(), "Failed to open file");
        }
    }

    ~FileDataWriter()
    {
        if (_fd != -1) {
            ::close(_fd);
        }
    }

    void write(std::string_view data)
    {
        size_t total_written = 0;
        while (total_written < data.size()) {
            ssize_t bytes_written =
                    ::write(_fd, data.data() + total_written, data.size() - total_written);
            if (bytes_written == -1) {
                throw std::system_error(errno, std::generic_category(), "Failed to write to file");
            }
            total_written += bytes_written;
        }
    }

private:
    int _fd; // File descriptor
};
} // namespace mmd

#endif // FILEDATAWRITER_HPP_