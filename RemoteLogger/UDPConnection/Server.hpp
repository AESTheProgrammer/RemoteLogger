#pragma once

#ifndef REMOTELOGGER_UDPCONNECTION_SERVER_HPP
#define REMOTELOGGER_UDPCONNECTION_SERVER_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>

#include "Acceptor.hpp"

using namespace boost;

namespace UDPConnection
{
    class Server
    {
    public:
        Server();
        void start(unsigned short port_num);
        void stop();
    private:
        void run(unsigned short port_num);
        std::unique_ptr<std::thread> m_thread;
        std::atomic<bool> m_stop;
        asio::io_service m_ios;
    };
}

#endif