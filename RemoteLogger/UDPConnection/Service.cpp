#include "Service.hpp"

using namespace UDPConnection;

Service::Service() 
{
}

Service::~Service() 
{
}

void Service::handleClient(asio::ip::udp::socket &sock, asio::ip::udp::endpoint *ep)
{
	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open())
	{
		std::string tmp;
		std::cout << "creating producer \n";
		std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
		// std::cout << "creating UDP consumer \n";
		// std::thread consumer(writeToUDPSock, std::ref(sock), std::ref(ep), std::ref(q));
		// std::cout << "waiting for UDP consumer \n";
		//consumer.join();
		std::string buf;
		while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000)))) {
			sock.send_to(boost::asio::buffer(buf + "\n"), *ep, 0);
			#if DEBUG
			cout << (count++);
			cout << buf + "\n";
			#endif 
		}
		std::cout << "waiting for producer \n";
		producer.join();
	}
	else
	{
		std::cout << "could not Open the Log file.\n";
	}
}
