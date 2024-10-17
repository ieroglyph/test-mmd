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

template<typename T>
concept Formatter = requires(T t, const ReceivedData &rd)
{
    {
        t.format(rd)
    } -> std::same_as<FormattedData>;
};

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
        using namespace fmt::literals;
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
    bool _stop{ false };
};
} // namespace mmd

#endif // FORMATWORKER_HPP_