#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <chrono>
#include <cstring>

#include <lockfree.hpp>

namespace mmd {
namespace config {

static constexpr size_t buf_size{ 1500 }; // ideally, size of MTU in the system

} // namespace config

struct ReceivedData
{
    ReceivedData(size_t address, char *datap, size_t datas)
        : timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
        , address(address)
        , datasize(datas)
    {
        memcpy(&data, datap, datas);
    }
    ReceivedData() = default;
    size_t timestamp;
    size_t address;
    size_t datasize;
    char data[config::buf_size];
};

struct FormattedData
{
    size_t datasize;
    char data[config::buf_size * 3];
};

using ReceiverQueue = lockfree::spsc::Queue<ReceivedData, 64>;
using FormatterQueue = lockfree ::spsc::Queue<FormattedData, 64>;
} // namespace mmd

#endif // COMMON_HPP_