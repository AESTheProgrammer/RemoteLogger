// RemoteLogger.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <ctime>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/array.hpp>
#include <boost/asio/ssl.hpp>

#include "RemoteLogger.hpp"
#include "SSLConnection/Acceptor.hpp"
#include "SSLConnection/Service.hpp"
#include "SSLConnection/Server.hpp"
#include "blockingconcurrentqueue.h"
#include "FileUtil.hpp"
#include "DBUtil.hpp"

#define DEBUG 0

using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;
using namespace boost;
using namespace SSLConnection;

void writeToTCPSock(asio::ip::tcp::socket &sock, cQueue &q)
{
	std::string buf;
	int count = 1;
	while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000)))) {
		asio::write(sock, asio::buffer(buf + "\n"));
		#if DEBUG
		cout << (count++);
		cout << buf + "\n";
		#endif 
	}
}


int sendSavedLogsTCP(asio::ip::tcp::socket &sock, asio::ip::udp::endpoint *ep = nullptr)
{
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open())
	{
		std::string tmp;
		std::cout << "creating producer \n";
		//std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
		DBUtil* db = DBUtil::getInstance();
		std::thread producer(&DBUtil::getLogs, db, std::ref(q));
		std::cout << "creating TCP consumer \n";
		std::thread consumer(writeToTCPSock, std::ref(sock), std::ref(q));
		std::cout << "waiting for TCP consumer \n";
		consumer.join();
		std::cout << "waiting for producer \n";
		producer.join();
	}
	else
	{
		std::cout << "could not Open the Log file.\n";
	}
	return 0;
}

int TCPServer()
{

	const int BACKLOG_SIZE = 30;
	unsigned short port_num = 3333;
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(),
							   port_num);
	asio::io_service ios;
	try
	{
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
		acceptor.bind(ep);
		std::cout << "TCP server started\n";
		while (true)
		{
			acceptor.listen(BACKLOG_SIZE);
			asio::ip::tcp::socket sock(ios);
			acceptor.accept(sock);
			sendSavedLogsTCP(sock);
			std::cout << "Finished sending logs\n";
		}
	}
	catch (system::system_error &e)
	{
		std::cout << "Error occured! Error code = " << e.code()
				  << ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

void writeToUDPSock(asio::ip::udp::socket &sock,
					asio::ip::udp::endpoint *&ep,
					cQueue &q)
{
	std::string buf;
	while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000)))) {
		sock.send_to(boost::asio::buffer(buf + "\n"), *ep, 0);
		#if DEBUG
		cout << (count++);
		cout << buf + "\n";
		#endif 
	}
}

int sendSavedLogsUDP(asio::ip::udp::socket &sock, asio::ip::udp::endpoint *ep = nullptr)
{
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open())
	{
		std::string tmp;
		std::cout << "creating producer \n";
		std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
		std::cout << "creating UDP consumer \n";
		std::thread consumer(writeToUDPSock, std::ref(sock), std::ref(ep), std::ref(q));
		std::cout << "waiting for UDP consumer \n";
		consumer.join();
		std::cout << "waiting for producer \n";
		producer.join();
	}
	else
	{
		std::cout << "could not Open the Log file.\n";
	}
	return 0;
}

int UDPServer()
{
	try
	{
		asio::io_service io_service;
		unsigned short port_num = 3334;
		asio::ip::udp::socket sock(	io_service,
								   	asio::ip::udp::endpoint(
										asio::ip::address_v4::any(), port_num));
		std::cout << "UDP server started\n";
		while (true)
		{
			boost::array<char, 1> recv_buf;
			asio::ip::udp::endpoint *ep = new asio::ip::udp::endpoint();
			sock.receive_from(boost::asio::buffer(recv_buf), *ep);
			sendSavedLogsUDP(sock, ep);
			delete ep;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << "\n";
	}
	std::cout << "Finished sending logs UDP\n";
	return 0;
}

void SSLServer() {
	unsigned short port_num = 3335;
	try {
		Server srv;
		srv.start(port_num);
		srv.stop();
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = "
			<< e.code() << ". Message: "
			<< e.what();
	}
}

void fillDB() {
	DBUtil* db = DBUtil::getInstance();
	//db.print();
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	cQueue q;
	readFileToQueue(logFileName, q);
	std::string buf;
	std::vector<string> logs;
	while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(100))))
		logs.push_back(buf);
	db->insertLogs(logs);
}

int main()
{
	std::thread tcpHandler(TCPServer);
	std::thread udpHandler(UDPServer);
	// std::thread sslHandler(SSLServer);
	tcpHandler.join();
	udpHandler.join();
	// sslHandler.join();
	return 0;
}

