#include "Client.hpp"

int main() {
    try {
        boost::asio::io_service io_service;
        Client client(io_service, "localhost", "12345");
        io_service.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
