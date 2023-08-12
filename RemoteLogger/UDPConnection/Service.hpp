#pragma once

#ifndef REMOTELOGGER_UDPCONNECTION_SERVICE_HPP
#define REMOTELOGGER_UDPCONNECTION_SERVICE_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "blockingconcurrentqueue.h"

#include "../Util/FileUtil.hpp"
#include "../Util/DBUtil.hpp"

using namespace boost;

using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;

namespace UDPConnection
{
    class Service
    {
    public:
        Service();
        ~Service();
        void handleClient(asio::ip::udp::socket& sock, asio::ip::udp::endpoint* ep = nullptr);
    };
}
#endif