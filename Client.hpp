#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread> 

class Client {
public:
    Client(boost::asio::io_service& io_service, const std::string& host, const std::string& port);
    void write(const std::string& message);
    void close();

private:
    void connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void handleConnect(const boost::system::error_code& error);
    void read();
    void handleRead(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);
    void startInputThread();

    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _response;
    std::string _write_message;
    std::thread _input_thread; 
};
