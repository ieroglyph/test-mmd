#include <fmt/compile.h>
#include <fmt/core.h>

#include <iostream>
#include <string>
#include <string_view>
#include <termios.h>
#include <thread>
#include <unistd.h>

#include "BasicFormatter.hpp"
#include "Common.hpp"
#include "FileDataWriter.hpp"
#include "FormatWorker.hpp"
#include "Params.hpp"
#include "UdpServer.hpp"

/**
 * @brief Pauses the main thread and waits for input from the user. 
 * If the user enters 'q', the program will quit.
 * If the \p signal_quit becomes true, the function will return immediately.
 * @param signal_quit 
 */
void waitForQuitSignal(bool &signal_quit)
{
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // Get current terminal settings
    newt = oldt; // Copy them to a new struct
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply the new settings
    while (!signal_quit) {
        char ch;
        std::cin >> ch; // Read a single character
        if (ch == 'q') signal_quit = true;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore original settings
}

int main(int argc, char **argv)
try {
    mmd::Params params(argc, argv);
    if (params.requestedHelp()) {
        fmt::print("{}\n", params.help());
        return 0;
    }

    fmt::print("Starting server at {}:{} with filter string {}\nWriting log to {}\n",
               params.address(), params.port(), params.filter(), params.filename());

    // init the queues to transfer from server to formatter to writer
    mmd::ReceiverQueue rqueue;
    mmd::FormatterQueue fqueue;

    // global signal to stop things
    bool signal_quit{ false };

    // formatting part
    // separated to make things spicier, and to test it properly
    // runs in it's own thread
    // Separating formatter and queues helps with mocking, yada, yada, yada
    // Any exception stops all things
    mmd::BasicFormatter formatter;
    mmd::FormatWorker fworker(formatter, rqueue, fqueue);
    auto fthread = std::thread([&fworker, &signal_quit] {
        try {
            fworker.run();
        } catch (std::exception &e) {
            fmt::print("fthread failed with exception: {}", e.what());
            signal_quit = true;
        }
    });

    // server part
    // the same approach here, but only the queue is moved to templated args
    // ideally the server should be separated from worker, same as with formatter
    // Any exception stops all things
    mmd::UdpServer server(params.address(), params.port(), rqueue);
    auto sthread = std::thread([&server, &signal_quit] {
        try {
            server.run();
        } catch (std::exception &e) {
            fmt::print("sthread failed with exception: {}", e.what());
            signal_quit = true;
        }
    });

    // file writing part
    // runs in it's own thread, apparently
    // could be the same as formatter and server thing, with its own tests, but ¯\_(ツ)_/¯
    // Any exception stops all things
    auto wthread = std::thread([filename = params.filename(), &fqueue, &signal_quit] {
        try {
            mmd::FileDataWriter fwriter(filename);
            while (!signal_quit) {
                const auto i = fqueue.PopOptional();
                if (i) {
                    fwriter.write({ i.value().data, i.value().datasize });
                }
            }
        } catch (std::exception &e) {
            fmt::print("wthread failed with exception: {}", e.what());
            signal_quit = true;
        }
    });

    fmt::print("q to quit\n");
    waitForQuitSignal(signal_quit);

    fmt::print("Stopping work; press ^C to force stop\n");
    server.stop();
    fworker.stop();

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