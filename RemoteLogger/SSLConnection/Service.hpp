#pragma once

#ifndef REMOTELOGGER_SSLCONNECTION_SERVICE_HPP
#define REMOTELOGGER_SSLCONNECTION_SERVICE_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include "blockingconcurrentqueue.h"

#include "../FileUtil.hpp"

using namespace boost;

using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;

namespace SSLConnection
{
    class Service
    {
    public:
        Service();
        ~Service();
        void handleClient(
            asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream);
    };
}
#endif