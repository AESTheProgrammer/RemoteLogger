#include "Server.hpp"

using namespace UDPConnection;

Server::Server() : m_stop(false) {}

void Server::start(unsigned short port_num)
{
    m_thread.reset(new std::thread([this, port_num]()
                                   { run(port_num); }));
}

void Server::stop()
{
    m_stop.store(true);
    m_thread->join();
}

void Server::run(unsigned short port_num)
{

    Acceptor acc(m_ios, port_num);
	std::cout << "UDP server started\n";
    while (!m_stop.load())
        acc.accept();
}