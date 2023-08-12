#include "Acceptor.hpp"
#include <memory>

using namespace UDPConnection;

Acceptor::Acceptor(asio::io_service &ios, unsigned short port_num)
    : m_ios(ios),
      m_acceptor(m_ios,
                 asio::ip::udp::endpoint(
                     asio::ip::address_v4::any(),
                     port_num))
{
}

void Acceptor::accept()
{
	Service svc;
	try
	{

		boost::array<char, 1> recv_buf;
		asio::ip::udp::endpoint* ep = new asio::ip::udp::endpoint();
		m_acceptor.receive_from(boost::asio::buffer(recv_buf), *ep);
		svc.handleClient(m_acceptor, ep);
		delete ep;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	std::cout << "Finished sending logs UDP\n";
}