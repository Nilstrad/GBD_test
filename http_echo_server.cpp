#include <iostream>
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

void handle_request(tcp::socket &socket) {
    try {
        char data[1024];
        boost::system::error_code error;

        // Читаем данные из сокета
        size_t length = socket.read_some(boost::asio::buffer(data), error);
        if (error == boost::asio::error::eof)
            return;
        else if (error)
            throw boost::system::system_error(error);

        // Преобразуем данные в строку
        std::string request(data, length);

        // Формируем HTTP-ответ
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Connection: close\r\n\r\n";
        response += "Echo: " + request;

        // Отправляем ответ клиенту
        boost::asio::write(socket, boost::asio::buffer(response), error);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    try {
        io_service io_service;

        // Создаем сервер
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));

        while (true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            handle_request(socket);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
