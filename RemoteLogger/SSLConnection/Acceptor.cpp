#include "Acceptor.hpp"
#include <memory>

using namespace SSLConnection;

Acceptor::Acceptor(asio::io_service &ios, unsigned short port_num)
    : m_ios(ios),
      m_acceptor(m_ios,
                 asio::ip::tcp::endpoint(
                     asio::ip::address_v4::any(),
                     port_num)),
      m_ssl_context(asio::ssl::context::sslv23_server)
{
    // Setting up the context.
    m_ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds | 
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use);
    m_ssl_context.set_password_callback(
        [this](std::size_t max_length,
               asio::ssl::context::password_purpose purpose)
            -> std::string
        { return get_password(max_length, purpose); });

    m_ssl_context.use_certificate_chain_file("server.crt");
    m_ssl_context.use_private_key_file("server.key",
                                       boost::asio::ssl::context::pem);
    m_ssl_context.use_tmp_dh_file("dhparams.pem");
    // Start listening for incoming connection requests.
    m_acceptor.listen();
}

void Acceptor::accept()
{
    asio::ssl::stream<asio::ip::tcp::socket>
        ssl_stream(m_ios, m_ssl_context);
    m_acceptor.accept(ssl_stream.lowest_layer());
    Service svc;
    svc.handleClient(ssl_stream);
}

std::string Acceptor::get_password(std::size_t max_length,
                                   asio::ssl::context::password_purpose purpose) const
{
    return "pass";
}