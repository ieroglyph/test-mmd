#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <functional>

#include "Common.hpp"

using asio::ip::udp;

namespace mmd {
class UdpServer
{
public:
    UdpServer(short port, ReceiverQueue &queue)
        : _io_context()
        , _socket(_io_context, udp::endpoint(udp::v4(), port))
        , _queue{ queue }
    {
        do_receive();
    }
    void run()
    {
        _io_context.run();
    }

private:
    void do_receive()
    {
        const auto callback = [this](std::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                [[maybe_unused]] const auto pr = _queue.get().Push(
                        { _sender.address().to_v4().to_uint(), _data, bytes_recvd });
#ifdef BENCHMARK_LOGS
                if (!pr) fmt::print("sfull\n");
#endif
            }
            do_receive();
        };
        _socket.async_receive_from(asio::buffer(_data, config::buf_size), _sender, callback);
    }
    asio::io_context _io_context;
    udp::socket _socket;
    udp::endpoint _sender;
    char _data[config::buf_size];
    std::reference_wrapper<ReceiverQueue> _queue;
};
} // namespace mmd

#endif // UDPSERVER_HPP_