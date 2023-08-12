#pragma once

#ifndef REMOTELOGGER_TCPCONNECTION_ACCEPTOR_HPP
#define REMOTELOGGER_TCPCONNECTION_ACCEPTOR_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Service.hpp"

using namespace boost;
namespace TCPConnection
{
    class Acceptor
    {
    public:
        Acceptor(asio::io_service &ios, unsigned short port_num);
        void accept();
    private:
        asio::io_service &m_ios;
        asio::ip::tcp::acceptor m_acceptor;
    };
}

#endif