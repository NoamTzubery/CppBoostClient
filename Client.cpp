#include "Client.hpp"

Client::Client(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
    : _socket(io_service) {
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve(host, port);
    connect(endpoints);
}

void Client::connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket, endpoints,
        [this](const boost::system::error_code& error, const boost::asio::ip::tcp::endpoint&) {
            handleConnect(error);
        });
}

void Client::handleConnect(const boost::system::error_code& error) {
    if (!error) {
        std::cout << "Connected to the server." << std::endl;
        read();
        startInputThread();  
    }
    else {
        std::cerr << "Connect error: " << error.message() << std::endl;
    }
}

void Client::startInputThread() {
    _input_thread = std::thread([this]() {
        std::string message;
        while (std::getline(std::cin, message)) {
            if (message == "exit") {
                close();
                break;
            }
            write(message);
        }
        });
    _input_thread.detach();
}

void Client::read() {
    boost::asio::async_read_until(_socket, _response, '\n',
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handleRead(error, bytes_transferred);
        });
}

void Client::handleRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::istream response_stream(&_response);
        std::string response;
        std::getline(response_stream, response);
        std::cout << "Received: " << response << std::endl;
        read();  // Continue to read next message
    }
    else {
        std::cerr << "Read error: " << error.message() << std::endl;
        close();
    }
}

void Client::write(const std::string& message) {
    auto msg_with_newline = message + '\n';  // Ensure the message is newline-terminated
    _write_message = msg_with_newline;
    boost::asio::async_write(_socket, boost::asio::buffer(_write_message),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handleWrite(error, bytes_transferred);
        });
}

void Client::handleWrite(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        std::cerr << "Write error: " << error.message() << std::endl;
        close();
    }
}

void Client::close() {
    _socket.close();
    std::cout << "Connection closed." << std::endl;
}
