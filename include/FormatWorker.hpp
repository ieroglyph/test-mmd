/**
 * @file FormatWorker.hpp
 * @brief Contains implementation of a worker that formats received data.
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef FORMATWORKER_HPP_
#define FORMATWORKER_HPP_

#include <concepts>
#include <span>

#include <fmt/base.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <lockfree.hpp>

#include "Common.hpp"

namespace mmd {

/**
 * @brief Formatter concept
 * Used to restrict the formatters that can be used in the FormatWorker.
 * @tparam T 
 */
template<typename T>
concept Formatter = requires(T t, const ReceivedData &rd)
{
    {
        t.format(rd)
    } -> std::same_as<FormattedData>;
};

/**
 * @brief Construct a new requires object
 * A super-basic worker that delegates all work to formatters and queues.
 * Relays on the non-blocking thread grinding.
 * 
 * @tparam FormatterT Something that implements the Formatter concept.
 * @tparam RQueueT Something that is a queue with ReceivedData.
 * @tparam FQueueT Something that is a queue with FormattedData.
 */
template<Formatter FormatterT, typename RQueueT, typename FQueueT>
requires(Queue<RQueueT, ReceivedData> &&Queue<FQueueT, FormattedData>) class FormatWorker
{
public:
    FormatWorker(FormatterT &formatter, RQueueT &rqueue, FQueueT &fqueue)
        : _formatter{ formatter }
        , _rqueue{ rqueue }
        , _fqueue{ fqueue }
    {
    }

    void run()
    {
        while (!_stop) {
            if (const auto rec = _rqueue.get().PopOptional(); rec) {
                auto rdata = rec.value();
                auto fdata = _formatter.get().format(rdata);
                [[maybe_unused]] const auto pr = _fqueue.get().Push(std::move(fdata));
#ifdef BENCHMARK_LOGS
                if (!pr) fmt::print("ffull\n");
#endif
            }
        }
    }

    void stop()
    {
        _stop = true;
    }

private:
    std::reference_wrapper<FormatterT> _formatter;
    std::reference_wrapper<RQueueT> _rqueue;
    std::reference_wrapper<FQueueT> _fqueue;
    bool _stop{ false }; // no reason to have it atomic
};
} // namespace mmd

#endif // FORMATWORKER_HPP_