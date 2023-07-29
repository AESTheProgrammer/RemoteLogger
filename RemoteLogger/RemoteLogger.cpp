// RemoteLogger.cpp : Defines the entry point for the application.
//

#include "RemoteLogger.h"
#include "blockingconcurrentqueue.h"

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

using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;
using namespace boost;

void writeToTCPSock(asio::ip::tcp::socket& sock, cQueue& q) 
{
	std::string buf;
	while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000))))
		asio::write(sock, asio::buffer(buf + "\n"));
}

void readFileToQueue(const char (&fileName)[], cQueue& q)
{
	std::ifstream file{fileName};
	std::string tmp;
	while (std::getline(file, tmp))
		q.enqueue(std::move(tmp));
}

int sendSavedLogsTCP(asio::ip::tcp::socket& sock, asio::ip::udp::endpoint* ep = nullptr)
{
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open()) {
		std::string tmp;
		std::cout << "creating producer \n";
		std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
		std::cout << "creating TCP consumer \n";
		std::thread consumer(writeToTCPSock, std::ref(sock), std::ref(q));
		std::cout << "waiting for TCP consumer \n";
		consumer.join();
		std::cout << "waiting for producer \n";
		producer.join();
	} else {
		std::cout << "could not Open the Log file." << std::endl;
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
	try {
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
		acceptor.bind(ep);
		std::cout << "TCP server started\n";
		while (true) {
			acceptor.listen(BACKLOG_SIZE);
			asio::ip::tcp::socket sock(ios);
			acceptor.accept(sock);
			//sendSavedLogs<asio::ip::tcp::socket>(sock);
			sendSavedLogsTCP(sock);
		}
	} catch (system::system_error &e) {
		std::cout << "Error occured! Error code = " << e.code()
		<< ". Message: " << e.what();
		return e.code().value();
	}
	std::cout << "Finished sending logs\n";
	return 0;
}

void writeToUDPSock(asio::ip::udp::socket& sock,
					asio::ip::udp::endpoint*& ep,
					cQueue& q)
{
	std::string buf;
	//boost::system::error_code e;
	//std::cout << (*ep).address() << "   " << ep->port();
	while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000)))) {
		//std::cout << buf + "\n";
		sock.send_to(boost::asio::buffer(buf + "\n"), *ep, 0);
	}
}

int sendSavedLogsUDP(asio::ip::udp::socket& sock, asio::ip::udp::endpoint* ep = nullptr)
{
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open()) {
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
	else {
		std::cout << "could not Open the Log file." << std::endl;
	}
	return 0;
}

int UDPServer()
{

	try
	{
		asio::io_service io_service;
		unsigned short port_num = 3334;
		asio::ip::udp::socket sock(io_service, 
		asio::ip::udp::endpoint(asio::ip::address_v4::any(), port_num));
		std::cout << "UDP server started\n";
		while (true)
		{
			boost::array<char, 1> recv_buf;
			asio::ip::udp::endpoint* ep = new asio::ip::udp::endpoint();
			sock.receive_from(boost::asio::buffer(recv_buf), *ep);
			//sendSavedLogs<asio::ip::udp::socket>(sock, ep);
			sendSavedLogsUDP(sock, ep);
			delete ep;
		}
	}
	catch (std::exception& e)
	{
		//std::cout << "Error occured! Error code = " << e.code()
		//<< ". Message: " << e.what();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Finished sending logs UDP\n";
	return 0;
}

int main()
{
	/*
	int isClient;
	std::cout << "0: UDP server\n1: TCP server\n";
	std::cin >> isClient;
	if (isClient == 1) {
		TCPServer();
	} else if (isClient == 0) {
		UDPServer();
	}*/
	std::thread tcpHandler(TCPServer);
	std::thread udpHandler(UDPServer);
	tcpHandler.join();
	udpHandler.join();
	return 0;
}


//std::string readFromSockUntil(asio::ip::tcp::socket& sock) 
//{
//	asio::streambuf buf;
//	std::string message;
//	while ((asio::read_until(sock, buf, '\n'))) {
//		std::istream input_stream(&buf);
//		std::getline(input_stream, message);
//		std::cout << message << '\n';
//	}
//	return message;
//}
//
//
//int client()
//{
//	std::string raw_ip_address = "127.0.0.1";
//	unsigned short port_num = 3333;
//	boost::system::error_code ec;
//	try {
//		asio::ip::tcp::endpoint ep(
//			asio::ip::address::from_string(raw_ip_address),
//			port_num);
//		asio::io_service ios;
//		asio::ip::tcp::socket sock(ios, ep.protocol());
//		sock.connect(ep);
//		readFromSockUntil(sock);
//	} catch(system::system_error &e) {
//		std::cout << "Error occured! Error code = " << e.code()
//			<< ". Message: " << e.what();
//		return e.code().value();
//	}
//	return 0;
//}

//template<typename Socket>
//int sendSavedLogs(Socket& sock, asio::ip::udp::endpoint* ep = nullptr)
//{
//	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
//	std::ifstream logFile{logFileName};
//	cQueue q;
//	if (logFile.is_open()) {
//		std::string tmp;
//		std::cout << "creating producer \n";
//		std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
//		if (typeid(asio::ip::tcp::socket) == typeid(Socket)) {
//			std::cout << "creating TCP consumer \n";
//			std::thread consumer(writeToTCPSock, std::ref(static_cast<asio::ip::tcp::socket>(sock)), std::ref(q));
//			std::cout << "waiting for TCP consumer \n";
//			consumer.join();
//		}
//		else {
//			//std::cout << "creating UDP consumer \n";
//			//std::thread consumer(writeToUDPSock, std::ref(sock), std::ref(ep), std::ref(q));
//			//std::cout << "waiting for UDP consumer \n";
//			//consumer.join();
//		}
//		//std::cout << "thread for producing logs\n";
//		//std::thread newLogProducer(writeToFile, std::ref(q), logFileName);
//		std::cout << "waiting for producer \n";
//		producer.join();
//		//newLogProducer.join();
//		//std::cout << "waiting for consumer \n";
//		//consumer.join();
//	} else {
//		std::cout << "could not Open the Log file." << std::endl;
//	}
//	return 0;
//}
//



//void writeToFile(cQueue& q, const std::string& logFileName)
//{
//	std::vector<std::string> newLogs = {
//	"[2019-02-11 22:01:55.607] [0x700000de4000] [DEBUG] new log from no where!\n",
//	"[2019-02-11 22:01:55.611] [0x700000d61000] [DEBUG] Hello from thread #45 Iteration #7\n",
//	"[2019-02-11 22:01:55.608] [0x700000cde000] [ERROR] Interesting log11\n"
//	};
//	std::ofstream os(logFileName, std::ios_base::app);
//	while (true) {
//		std::this_thread::sleep_for(std::chrono::seconds(5));
//		for (auto newLog : newLogs) {
//			os << newLog;
//		}
//	}
//}
//

//using namespace boost::asio;
//using namespace boost::asio::ip;

//void waitForConnection() {
//	boost::asio::io_service io_service;
//	//listen for new connection
//	tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));
//	//socket creation 
//	tcp::socket socket_(io_service);
//	//waiting for connection
//	acceptor_.accept(socket_);
//	//read operation
//	std::string message = read_(socket_);
//	std::cout << message << '\n';
//	//write operation
//	send_(socket_, "Hello From Server!");
//	cout << "Servent sent Hello message to Client!" << endl;
//   return 0;
//}
//
//template<typename T>
//void swap(T& a, T& b) {
//	T tmp(std::move(a));
//	a = std::move(b);
//	b = std::move(tmp);
//}
//

// io_service ioservice1;
// tcp::resolver resolv{ioservice1};
// tcp::socket tcp_client_socket{ioservice1};
// std::array<char, 4096> bytes;

// io_service ioservice2;
// tcp::endpoint tcp_endpoint{tcp::v4(), 2014};
// tcp::acceptor tcp_acceptor{ioservice2, tcp_endpoint};
// tcp::socket tcp_server_socket{ioservice2};
// std::string data;


// // client
// void read_handler(const boost::system::error_code &ec,
//   std::size_t bytes_transferred)
// {
//   if (!ec)
//   {
//     std::cout.write(bytes.data(), bytes_transferred);
//     tcp_client_socket.async_read_some(buffer(bytes), read_handler);
//   }
// }

// void connect_handler(const boost::system::error_code &ec)
// {
//   if (!ec)
//   {
//     std::string r =
//       "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
//     write(tcp_client_socket, buffer(r));
//     tcp_client_socket.async_read_some(buffer(bytes), read_handler);
//   }
// }

// void resolve_handler(const boost::system::error_code &ec,
//   tcp::resolver::iterator it)
// {
//   if (!ec)
//     tcp_client_socket.async_connect(*it, connect_handler);
// }

// void client()
// {
//     // tcp::resolver::query q{"google.com", "80"};
// 	tcp_client_socket.connect(tcp::endpoint(
// 		boost::asio::ip::address::from_string("127.0.0.1"), 2014));
//     // resolv.async_resolve(q, resolve_handler);
// 	// request/message from client
//      const std::string msg = "Hello from Client!\n";
//      boost::system::error_code error;
//      write( tcp_client_socket, boost::asio::buffer(msg), error );
//      if( !error ) {
//         std::cout << "Client sent hello message!" << std::endl;
//      }
//      else {
//         std::cout << "send failed: " << error.message() << std::endl;
//      }
//  // getting response from server
//     streambuf receive_buffer;
//     read(tcp_client_socket, receive_buffer, boost::asio::transfer_all(), error);
//     if( error && error != boost::asio::error::eof ) {
//         std::cout << "receive failed: " << error.message() << std::endl;
//     }
//     else {
//         const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
//         std::cout << data << std::endl;
//     }
//     int num;
//     std::cin >> num;
//     //ioservice1.run();
// }

// // server
// void write_handler(const boost::system::error_code &ec,
//   std::size_t bytes_transferred)
// {
//   if (!ec)
//     tcp_server_socket.shutdown(tcp::socket::shutdown_send);
// }

// void accept_handler(const boost::system::error_code &ec)
// {
//   if (!ec)
//   {
//     std::time_t now = std::time(nullptr);
//     data = std::ctime(&now);
//     async_write(tcp_server_socket, buffer(data), write_handler);
//   }
// }


// void server()
// {
//     tcp_acceptor.listen();
//     tcp_acceptor.async_accept(tcp_server_socket, accept_handler);
//     ioservice2.run();
// }

// int old_client() {
// 	std::string raw_ip_address = "127.0.0.1";
// 	unsigned short port_num = 3333;
// 	boost::system::error_code ec;
// 	asio::ip::address ip_address = 
// 		asio::ip::address::from_string(raw_ip_address, ec);
// 	if (ec.value() != 0) {
// 		std::cout 
// 		<< "Failed to parse the IP address. Error code = "
// 		<< ec.value() << ". Message: " << ec.message();
// 		return ec.value();
// 	}
// 	asio::ip::tcp::endpoint ep(ip_address, port_num);
// 	//asio::ip::udp::endpoint ep(ip_address, port_num);

// 	asio::io_service ios;
// 	asio::ip::tcp protocol = asio::ip::tcp::v4();
// 	//asio::ip::udp protocol = asio::ip::udp::v4();
// 	asio::ip::tcp::socket sock(ios);
// 	//asio::ip::udp::socket sock(ios);
// 	sock.open(protocol, ec);
// 	if (ec.value() != 0) {
// 		std::cout
// 		<< "Failed to open the socket! Error code = "
// 		<< ec.value() << ". error Message: " << ec.message();
// 		return ec.value();
// 	}


// 	return 0;
// }

// int old_server() {
// 	unsigned short port_num = 3333;
// 	asio::ip::address ip_address = asio::ip::address_v4::any();
// 	asio::ip::tcp::endpoint ep(ip_address, port_num);
// 	//asio::ip::udp::endpoint ep(ip_address, port_num);

// 	asio::io_service ios;
// 	asio::ip::tcp protocol = asio::ip::tcp::v4();
// 	asio::ip::tcp::acceptor acceptor(ios);
// 	boost::system::error_code ec;
// 	acceptor.open(protocol, ec);
// 	if (ec.value() != 0) {
// 		std::cout
// 		<< "Failed to open the acceptor socket! Error code = "
// 		<< ec.value() << ". error Message: " << ec.message();
// 		return ec.value();
// 	}
// 	return 0;
// }

// int resolver() {
// 	std::string host = "google.com";
// 	std::string port_num = "80";
// 	asio::io_service ios;
// 	asio::ip::tcp::resolver::query resolver_query(host, 
// 		port_num, asio::ip::tcp::resolver::query::numeric_service);
// 	// asio::ip::udp::resolver::query resolver_query(host, 
// 	// 	port_num, asio::ip::udp::resolver::query::numeric_service);
// 	asio::ip::tcp::resolver resolver(ios);
// 	// asio::ip::udp::resolver resolver(ios);
// 	boost::system::error_code ec;
// 	asio::ip::tcp::resolver::iterator it =
// 		resolver.resolve(resolver_query, ec);
// 	// asio::ip::udp::resolver::iterator it =
// 	// 	resolver.resolve(resolver_query, ec);
// 	if (ec.value() != 0) {
// 		std::cout << "Failed to resolve a DNS name."
// 		<< "Error code = " << ec.value()
// 		<< ". Message = " << ec.message();
// 		return ec.value();
// 	}
// 	asio::ip::tcp::resolver::iterator it_end;
// 	for (; it != it_end; ++it) {
// 		asio::ip::tcp::endpoint ep = it->endpoint();
// 	}
// 	return 0;

// }