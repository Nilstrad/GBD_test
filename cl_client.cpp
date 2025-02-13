#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>

using namespace boost::asio;
using ip::tcp;
namespace json = boost::json;

// Отправка HTTP-запроса и получение ответа
std::string send_request(const std::string& request_body) {
    try {
        io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("127.0.0.1", "8080");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::socket socket(io_service);
        connect(socket, endpoint_iterator);

        std::string request = "POST / HTTP/1.1\r\n"
                              "Host: 127.0.0.1:8080\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: " + std::to_string(request_body.size()) + "\r\n"
                              "Connection: close\r\n\r\n" +
                              request_body;

        write(socket, buffer(request));

        boost::asio::streambuf response;
        read_until(socket, response, "\r\n\r\n");

        std::istream response_stream(&response);
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") { }

        std::string response_body;
        std::getline(response_stream, response_body, '\0');

        return response_body;
    } catch (std::exception& e) {
        return "{\"error\":\"" + std::string(e.what()) + "\"}";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " [-u <user>] -c <команда> | -e <выражение>" << std::endl;
        return 1;
    }

    json::object request_obj;
    std::string user;

    // Обработка аргументов командной строки. Флаг -u указывает пользователя.
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-u" && i + 1 < argc) {
            user = argv[++i];
        } else if (arg == "-c" && i + 1 < argc) {
            request_obj["cmd"] = argv[++i];
        } else if (arg == "-e" && i + 1 < argc) {
            std::string expression;
            for (int j = i + 1; j < argc; ++j) {
                if (j > i + 1)
                    expression += " ";
                expression += argv[j];
            }
            request_obj["exp"] = expression;
            break;
        }
    }

    // Если указан пользователь, добавляем его в JSON-запрос
    if (!user.empty())
        request_obj["user"] = user;

    std::string request_body = json::serialize(request_obj);
    std::string response = send_request(request_body);

    try {
        json::value parsed_response = json::parse(response);
        if (parsed_response.is_object()) {
            json::object res_obj = parsed_response.as_object();
            if (res_obj.contains("res")) {
                if (res_obj["res"].is_int64())
                    std::cout << res_obj["res"].as_int64() << std::endl;
                else if (res_obj["res"].is_double())
                    std::cout << res_obj["res"].as_double() << std::endl;
                else if (res_obj["res"].is_string())
                    std::cout << res_obj["res"].as_string() << std::endl;
                else
                    std::cerr << "Неизвестный тип результата" << std::endl;
            } else if (res_obj.contains("error")) {
                std::cerr << "Ошибка: " << res_obj["error"].as_string() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка обработки ответа: " << e.what() << std::endl;
    }

    return 0;
}
