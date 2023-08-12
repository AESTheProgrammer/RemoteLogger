#include "Acceptor.hpp"
#include <memory>

using namespace TCPConnection;

Acceptor::Acceptor(asio::io_service &ios, unsigned short port_num)
    : m_ios(ios),
      m_acceptor(m_ios,
                 asio::ip::tcp::endpoint(
                     asio::ip::address_v4::any(),
                     port_num))
{
    // Start listening for incoming connection requests.
    m_acceptor.listen();
}

void Acceptor::accept()
{
	asio::ip::tcp::socket sock(m_ios);
	m_acceptor.accept(sock);
    Service svc;
    svc.handleClient(sock);
}
