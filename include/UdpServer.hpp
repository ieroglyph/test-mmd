#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <functional>

#include "Common.hpp"

using asio::ip::udp;

namespace mmd {

template<typename RQueueT>
requires(Queue<RQueueT, ReceivedData>) class UdpServer
{
public:
    UdpServer(const std::string &address, short port, RQueueT &queue)
        : _context()
        , _socket(_context)
        , _queue{ queue }
    {
        auto addr = asio::ip::make_address(address);

        udp::endpoint ep(addr, port);
        _socket.open(ep.protocol());
        _socket.set_option(asio::ip::udp::socket::reuse_address(true));

        if (addr.is_multicast()) {
            _socket.set_option(asio::ip::multicast::join_group(addr));
        }
        _socket.bind(ep);
    }
    void run()
    {
        do_receive();
        _context.run();
    }
    void stop()
    {
        _socket.cancel();
        _context.stop();
    }

private:
    void do_receive()
    {
        const auto callback = [this](std::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                ReceivedData rdata{ _sender.address().to_string(), _data, bytes_recvd };
                [[maybe_unused]] const auto pr = _queue.get().Push(rdata);
#ifdef BENCHMARK_LOGS
                if (!pr) fmt::print("sfull\n");
#endif
            }
            do_receive();
        };
        _socket.async_receive_from(asio::buffer(_data, config::buf_size), _sender, callback);
    }
    asio::io_context _context;
    udp::socket _socket;
    udp::endpoint _sender;
    char _data[config::buf_size];
    std::reference_wrapper<RQueueT> _queue;
};
} // namespace mmd

#endif // UDPSERVER_HPP_