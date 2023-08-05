#include "Service.hpp"

using namespace SSLConnection;

Service::Service() 
{
}

Service::~Service() 
{
}

void Service::handleClient(
    asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream)
{
    const char logFileName[] = "C:\\Users\\armin\\Downloads\\MyLog.txt";
    std::ifstream logFile{logFileName};
    cQueue q;
    if (logFile.is_open())
    {
        try
        {
            // Blocks until the handshake completes.
            ssl_stream.handshake(
                asio::ssl::stream_base::server);
            std::cout << "creating producer \n";
            std::thread producer(readFileToQueue, std::ref(logFileName), std::ref(q));
            std::string buf;
            while ((q.wait_dequeue_timed(buf, std::chrono::milliseconds(1000))))
                asio::write(ssl_stream, asio::buffer(buf + "\n"));
            std::cout << "waiting for producer \n";
            producer.join();
        }
        catch (system::system_error &e)
        {
            std::cout << "Error occured! Error code = "
                      << e.code() << ". Message: "
                      << e.what();
        }
    }
    else
    {
        std::cout << "could not Open the Log file." << std::endl;
    }
}
