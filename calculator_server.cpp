#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace boost::asio;
using ip::tcp;
namespace json = boost::json;

double evaluate_expression(const std::string& expression) {
    std::istringstream iss(expression);
    double result, num;
    char op;

    if (!(iss >> result)) {
        throw std::invalid_argument("Invalid expression");
    }

    while (iss >> op >> num) {
        switch (op) {
            case '+': result += num; break;
            case '-': result -= num; break;
            case '*': result *= num; break;
            case '/':
                if (num == 0) throw std::runtime_error("Division by zero");
                result /= num;
                break;
            default: throw std::invalid_argument("Unsupported operator");
        }
    }

    return result;
}

void handle_request(tcp::socket& socket) {
    try {
        boost::asio::streambuf buffer;
        boost::system::error_code error;
        boost::asio::read_until(socket, buffer, "\r\n\r\n", error); // Читаем заголовки HTTP

        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        std::istream input_stream(&buffer);
        std::string request_line;
        std::getline(input_stream, request_line);

        std::string headers;
        std::string body;
        bool reading_body = false;
        size_t content_length = 0;

        while (std::getline(input_stream, request_line) && request_line != "\r") {
            if (request_line.find("Content-Length:") == 0) {
                content_length = std::stoi(request_line.substr(15));
            }
            headers += request_line + "\n";
        }

        if (content_length > 0) {
            std::vector<char> body_data(content_length);
            input_stream.read(body_data.data(), content_length);
            body.assign(body_data.begin(), body_data.end());
        }

        std::cout << "Received request body: " << body << std::endl;

        json::value parsed_request;
        try {
            parsed_request = json::parse(body);
        } catch (const std::exception& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            json::object response_object;
            response_object["error"] = "Invalid JSON format";
            std::string response = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Connection: close\r\n\r\n" +
                                   json::serialize(response_object);
            boost::asio::write(socket, boost::asio::buffer(response), error);
            return;
        }

        json::object response_object;

        if (parsed_request.is_object()) {
            json::object request_obj = parsed_request.as_object();

            if (request_obj.contains("cmd") && request_obj["cmd"].is_string()) {
                response_object["res"] = request_obj["cmd"];
            } else if (request_obj.contains("exp") && request_obj["exp"].is_string()) {
                std::string expression = json::value_to<std::string>(request_obj["exp"]);
                try {
                    double result = evaluate_expression(expression);

                    // Если результат целое число, сохраняем как int, иначе как double
                    if (std::floor(result) == result) {
                        response_object["res"] = static_cast<int>(result); // Преобразуем в int, если результат целое число
                    } else {
                        response_object["res"] = result; // Если вещественное число, оставляем как есть
                    }
                } catch (const std::exception& e) {
                    response_object["error"] = e.what();
                }
            } else {
                response_object["error"] = "Invalid request format";
            }
        } else {
            response_object["error"] = "Invalid JSON format";
        }

        // Сериализация объекта JSON с пробелами между ключом и значением
        std::ostringstream response_stream;
        response_stream << "{ ";
        bool first = true;
        for (const auto& kv : response_object) {
            if (!first) response_stream << ", ";
            response_stream << "\"" << kv.key() << "\": " << kv.value();
            first = false;
        }
        response_stream << " }";

        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Connection: close\r\n\r\n" +
                               response_stream.str();

        std::cout << "Sending response: " << response << std::endl;
        boost::asio::write(socket, boost::asio::buffer(response), error);
        socket.shutdown(tcp::socket::shutdown_both, error);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "HTTP Calculator Server is running on port 8080..." << std::endl;

        while (true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            handle_request(socket);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
