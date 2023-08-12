#pragma once

#ifndef REMOTELOGGER_UDPCONNECTION_ACCEPTOR_HPP
#define REMOTELOGGER_UDPCONNECTION_ACCEPTOR_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/array.hpp>

#include "Service.hpp"

using namespace boost;
namespace UDPConnection
{
    class Acceptor
    {
    public:
        Acceptor(asio::io_service &ios, unsigned short port_num);
        void accept();
    private:
        asio::io_service &m_ios;
        asio::ip::udp::socket m_acceptor;
    };
}

#endif