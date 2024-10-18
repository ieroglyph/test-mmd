/**
 * @file Common.hpp
 * @brief Some basic things used in more than one places
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <chrono>
#include <cstring>

#include <lockfree.hpp>

namespace mmd {
namespace config {

/**
 * @brief Buffer size for received data 
 * Ideally, size of MTU in the system
 */
static constexpr size_t buf_size{ 1500 }; 

} // namespace config

/**
 * @brief Represents things that would be received from the network
 * Made a trivial type for 
 * a) compatibility with queue and 
 * b) to have benefits of a trivial type
 */
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
    ReceivedData() = default; // to force triviality
    size_t timestamp; // TODO: we want better precision, perhaps?
    size_t addrsize;
    size_t datasize;
    char addr[8 * 4 + 8]; // is this the max length of a ipv6 address + 1?
    char data[config::buf_size];
};

/**
 * @brief Represents things that would be saved to a file
 * Also a trivial type
 */
struct FormattedData
{
    size_t datasize;
    char data[config::buf_size * 3]; // 2 per byte for hex encoding of 1 byte, plus "header", plus nalogi, plus dostavka, plus na pivo 
};

// Type aliases, saves keystrokes to write more comments
using ReceiverQueue = lockfree::spsc::Queue<ReceivedData, 64>;
using FormatterQueue = lockfree ::spsc::Queue<FormattedData, 64>;

/**
 * @brief Concept for cheking the Queue PopOptional and Push
 * Checks if the T can be used as a queue of Vs
 * @tparam T Queue type
 * @tparam V Value type
 */
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