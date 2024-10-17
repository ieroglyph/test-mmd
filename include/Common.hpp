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
    ReceivedData(const std::string &address, char *datap, size_t datas)
        : timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
        , addrsize(address.size())
        , datasize(datas)
    {
        memcpy(&addr, address.data(), addrsize);
        memcpy(&data, datap, datas);
    }
    ReceivedData() = default;
    size_t timestamp;
    size_t addrsize;
    size_t datasize;
    char addr[8 * 4 + 7]; // is this the max length of a ipv6 address?
    char data[config::buf_size];
};

struct FormattedData
{
    size_t datasize;
    char data[config::buf_size * 3];
};

using ReceiverQueue = lockfree::spsc::Queue<ReceivedData, 64>;
using FormatterQueue = lockfree ::spsc::Queue<FormattedData, 64>;

template<typename T, typename V>
concept Queue = requires(T t, V v)
{
    {
        t.PopOptional()
    } -> std::same_as<std::optional<V>>;
    {
        t.Push(v)
    } -> std::same_as<bool>;
};

static_assert(Queue<ReceiverQueue, ReceivedData>, "ReceiverQueue is supposed to be a Queue");
static_assert(Queue<FormatterQueue, FormattedData>, "FormatterQueue is supposed to be a Queue");

} // namespace mmd

#endif // COMMON_HPP_