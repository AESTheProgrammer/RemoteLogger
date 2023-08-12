#include "Service.hpp"

using namespace TCPConnection;

Service::Service() 
{
}

Service::~Service() 
{
}

void Service::handleClient(asio::ip::tcp::socket &sock) 
{

	const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
	std::ifstream logFile{logFileName};
	cQueue q;
	if (logFile.is_open())
	{
		std::string tmp;
		std::cout << "creating producer \n";
		std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
		// DBUtil *db = DBUtil::getInstance();
		// std::thread producer(&DBUtil::getLogs, db, std::ref(q));
		std::cout << "creating TCP consumer \n";
		// std::thread consumer(writeToTCPSock, std::ref(sock), std::ref(q));
        std::string buf;
        int count = 1;
        while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000)))) {
            asio::write(sock, asio::buffer(buf + "\n"));
            #if DEBUG
            cout << (count++);
            cout << buf + "\n";
            #endif 
        }
		//std::cout << "waiting for TCP consumer \n";
		//consumer.join();
		std::cout << "waiting for producer \n";
		producer.join();
	}
	else
	{
        std::cout << "could not Open the Log file." << std::endl;
	}
}