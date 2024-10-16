#include <fmt/compile.h>
#include <fmt/core.h>

#include <string>
#include <string_view>
#include <thread>

#include "BasicFormatter.hpp"
#include "Common.hpp"
#include "FileDataWriter.hpp"
#include "FormatWorker.hpp"
#include "Params.hpp"
#include "UdpServer.hpp"

int main(int argc, char **argv)
try {
    mmd::Params params(argc, argv);
    if (params.requestedHelp()) {
        fmt::print("{}\n", params.help());
        return 0;
    }

    const auto address = params.address();
    const auto port = params.port();
    const auto filter = params.filter();
    const auto filename = params.filename();

    fmt::print("Starting server at {}:{} with mask {}\nWriting log to {}\n", address, port, filter,
               filename);

    // init the queue to transfer from server to writer
    mmd::ReceiverQueue rqueue;
    mmd::FormatterQueue fqueue;

    // formatting part
    // separated to make things spicier, and to test it
    // runs in it's own thread
    mmd::BasicFormatter formatter;
    mmd::FormatWorker fworker(formatter, rqueue, fqueue);
    auto fthread = std::thread([&fworker] {
        fworker.run();
    });

    // server part
    mmd::UdpServer s(port, rqueue);
    auto sthread = std::thread([&s] {
        s.run();
    });

    // file writing part
    // runs in it's own thread, apparently
    // i'm too lazy to create a class for this worker
    mmd::FileDataWriter fwriter(filename);
    auto wthread = std::thread([&fwriter, &fqueue] {
        while (2 * 2 == 4) {
            const auto i = fqueue.PopOptional();
            if (i) {
                fwriter.write({ i.value().data, i.value().datasize });
            }
        }
    });

    // wait for threads to stop... on what condithin though?
    // TODO: add read from keyboard to stop things
    sthread.join();
    wthread.join();
    fthread.join();

} catch (const cxxopts::exceptions::exception &e) {
    fmt::print("Error parsing command line args. Try --help for usage details.");
    return 1;
} catch (const std::runtime_error &e) {
    fmt::print("Failed with exception: {}", e.what());
    return 2;
} catch (...) {
    fmt::print("Failed with unknown exception");
    return 3;
}