#pragma once

#ifndef REMOTELOGGER_SSLCONNECTION_ACCEPTOR_HPP
#define REMOTELOGGER_SSLCONNECTION_ACCEPTOR_HPP

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/array.hpp>
#include <boost/asio/ssl.hpp>

#include "Service.hpp"

using namespace boost;
namespace SSLConnection
{
    class Acceptor
    {
    public:
        Acceptor(asio::io_service &ios, unsigned short port_num);
        void accept();
    private:
        std::string get_password(std::size_t max_length,
                                 asio::ssl::context::password_purpose purpose) const;
    private:
        asio::io_service &m_ios;
        asio::ip::tcp::acceptor m_acceptor;
        asio::ssl::context m_ssl_context;
    };
}

#endif