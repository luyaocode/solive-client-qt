// BeastAdapter.h
#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <string>
#include "ISocket.h"

namespace beast = boost::beast;
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace SoLive::ProtocolSocketClient
{
    class BeastAdapter : public ISocket
    {
    public:
        BeastAdapter(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context);
        ~BeastAdapter() = default;

        void connect(const std::string& url) override;
        void emit(const std::string& eName, const std::string& msg) override;
        void listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback) override;
        void close() override;
        std::string id();

    private:
        websocket::stream<beast::ssl_stream<tcp::socket>> _ws;
    };
}
