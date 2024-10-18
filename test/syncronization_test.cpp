#include <catch2/catch_test_macros.hpp>

#include <exception>
#include <optional>
#include <stdexcept>
#include <thread>

#include "Common.hpp"
#include "FormatWorker.hpp"

using namespace mmd;

template<typename T>
class MockQueue
{
public:
    std::optional<T> PopOptional()
    {
        std::optional<T> result{ std::nullopt };
        const size_t count = _pop_count.load(std::memory_order::acquire);
        if (count % 3) { // to imitate that queue is sometimes empty
            _pop_count_true++;
            result = T{};
        } else {
            _pop_count_false++;
        }
        _pop_count.store(count + 1, std::memory_order::release);
        return result;
    };
    bool Push(const T &t)
    {
        _push_count++;
        return true;
    }
    size_t pop_count()
    {
        return _pop_count.load();
    }
    size_t pop_count_true()
    {
        return _pop_count_true.load();
    }
    size_t pop_count_false()
    {
        return _pop_count_false.load();
    }
    size_t push_count()
    {
        return _push_count.load();
    }

private:
    std::atomic<size_t> _pop_count_true{ 0 };
    std::atomic<size_t> _pop_count_false{ 0 };
    std::atomic<size_t> _push_count{ 0 };
    std::atomic<size_t> _pop_count{ 0 };
};

static_assert(Queue<MockQueue<int>, int>, "MockQueue is supposed to be mmd::Queue");

class MockFormatter
{
public:
    FormattedData format(const ReceivedData &)
    {
        _format_count++;
        return FormattedData{};
    }
    size_t format_count()
    {
        return _format_count.load();
    }

private:
    std::atomic<size_t> _format_count{ 0 };
};

static_assert(Formatter<MockFormatter>, "MockFormatter is expected to be a mmd::Formatter");

TEST_CASE("FormatWorker works as expected", "[synchronization]")
{
    GIVEN("Format worker created as usual")
    {
        MockFormatter formatter{};
        MockQueue<ReceivedData> rqueue{};
        MockQueue<FormattedData> fqueue{};

        FormatWorker fworker(formatter, rqueue, fqueue);

        WHEN("worker works for some time")
        {
            std::exception_ptr eptr = nullptr;

            auto wthread = std::thread([&fworker, &eptr] {
                try {
                    fworker.run();
                } catch (...) {
                    eptr = std::current_exception();
                }
            });

            std::this_thread::sleep_for(std::chrono::seconds(1));

            fworker.stop();
            wthread.join();

            THEN("no exceptions failed")
            {
                REQUIRE(!eptr);
            }
            AND_THEN("rqueue was never called for push")
            {
                REQUIRE(rqueue.push_count() == 0);
                REQUIRE(rqueue.pop_count() > 0);
            }
            AND_THEN("fqueue was never called for pop")
            {
                REQUIRE(fqueue.pop_count() == 0);
                REQUIRE(fqueue.push_count() > 0);
            }
            AND_THEN("operations were only called proper amount of times")
            {
                REQUIRE(formatter.format_count() > 0);
                REQUIRE(fqueue.push_count() == rqueue.pop_count_true());
                REQUIRE(fqueue.push_count() == formatter.format_count());
            }
        }
    }
}