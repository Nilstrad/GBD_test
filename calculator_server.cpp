#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <boost/asio.hpp>
#include <boost/json.hpp>

using namespace boost::asio;
using ip::tcp;
namespace json = boost::json;

// Определение типов токенов
enum class TokenType { Number, Identifier, Plus, Minus, Multiply, Divide, Equal, End };

struct Token {
    TokenType type;
    double value;          // для числовых литералов
    std::string text;      // для идентификаторов и операторов
};

// Функция токенизации входной строки
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < input.size()) {
        if (std::isspace(input[i])) {
            ++i;
            continue;
        }
        if (std::isdigit(input[i]) || input[i] == '.') {
            size_t start = i;
            while (i < input.size() && (std::isdigit(input[i]) || input[i] == '.')) {
                ++i;
            }
            double num = std::stod(input.substr(start, i - start));
            tokens.push_back({TokenType::Number, num, ""});
            continue;
        }
        if (std::isalpha(input[i])) {
            size_t start = i;
            while (i < input.size() && (std::isalnum(input[i]) || input[i]=='_')) {
                ++i;
            }
            std::string ident = input.substr(start, i - start);
            tokens.push_back({TokenType::Identifier, 0, ident});
            continue;
        }
        char ch = input[i];
        switch (ch) {
            case '+': tokens.push_back({TokenType::Plus, 0, "+"}); break;
            case '-': tokens.push_back({TokenType::Minus, 0, "-"}); break;
            case '*': tokens.push_back({TokenType::Multiply, 0, "*"}); break;
            case '/': tokens.push_back({TokenType::Divide, 0, "/"}); break;
            case '=': tokens.push_back({TokenType::Equal, 0, "="}); break;
            default:
                throw std::invalid_argument(std::string("Неожиданный символ: ") + ch);
        }
        ++i;
    }
    tokens.push_back({TokenType::End, 0, ""});
    return tokens;
}

// Парсер с поддержкой присваивания и арифметических операций
class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    // Обрабатывает последовательность выражений; результат – значение последнего
    double parse() {
        double result = 0;
        while (current().type != TokenType::End) {
            result = parseStatement();
        }
        return result;
    }
private:
    const std::vector<Token>& tokens;
    size_t pos;
    // Таблица переменных для текущего запроса
    std::unordered_map<std::string, double> variables;

    const Token& current() const {
        return tokens[pos];
    }
    
    // Обработка одного оператора: либо присваивание, либо арифметическое выражение
    double parseStatement() {
        if (current().type == TokenType::Identifier && tokens[pos+1].type == TokenType::Equal) {
            std::string varName = current().text;
            pos += 2; // пропускаем идентификатор и '='
            double value = parseExpression();
            variables[varName] = value;
            return value;
        } else {
            return parseExpression();
        }
    }

    // Выражение: сумма или разность термов
    double parseExpression() {
        double lhs = parseTerm();
        while (current().type == TokenType::Plus || current().type == TokenType::Minus) {
            Token op = current();
            ++pos;
            double rhs = parseTerm();
            if (op.type == TokenType::Plus)
                lhs += rhs;
            else
                lhs -= rhs;
        }
        return lhs;
    }

    // Термин: произведение или деление множителей
    double parseTerm() {
        double lhs = parseFactor();
        while (current().type == TokenType::Multiply || current().type == TokenType::Divide) {
            Token op = current();
            ++pos;
            double rhs = parseFactor();
            if (op.type == TokenType::Multiply)
                lhs *= rhs;
            else {
                if (rhs == 0)
                    throw std::runtime_error("Division by zero");
                lhs /= rhs;
            }
        }
        return lhs;
    }

    // Фактор: число или идентификатор (переменная)
    double parseFactor() {
        Token token = current();
        ++pos;
        if (token.type == TokenType::Number) {
            return token.value;
        } else if (token.type == TokenType::Identifier) {
            auto it = variables.find(token.text);
            if (it == variables.end())
                throw std::runtime_error("Unknown variable '" + token.text + "'");
            return it->second;
        } else if (token.type == TokenType::Minus) {
            return -parseFactor(); // поддержка унарного минуса
        }
        throw std::runtime_error("Unexpected token");
    }
};

// Обновлённая функция вычисления выражения с поддержкой переменных
double evaluate_expression(const std::string& expression) {
    std::vector<Token> tokens = tokenize(expression);
    Parser parser(tokens);
    return parser.parse();
}

// Обработка входящего HTTP-запроса
void handle_request(tcp::socket& socket) {
    try {
        boost::asio::streambuf buffer;
        boost::system::error_code error;
        // Читаем заголовки HTTP-запроса
        boost::asio::read_until(socket, buffer, "\r\n\r\n", error);
        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        std::istream input_stream(&buffer);
        std::string request_line;
        std::getline(input_stream, request_line);

        std::string headers;
        while (std::getline(input_stream, request_line) && request_line != "\r") {
            headers += request_line + "\n";
        }

        size_t content_length = 0;
        // Извлекаем Content-Length из заголовков
        std::istringstream headers_stream(headers);
        std::string header_line;
        while (std::getline(headers_stream, header_line)) {
            if (header_line.find("Content-Length:") != std::string::npos) {
                content_length = std::stoi(header_line.substr(header_line.find(":") + 1));
            }
        }

        std::string body;
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
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n" +
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
                    // Если результат целое число, выводим как int
                    if (std::floor(result) == result)
                        response_object["res"] = static_cast<int>(result);
                    else
                        response_object["res"] = result;
                } catch (const std::exception& e) {
                    response_object["error"] = e.what();
                }
            } else {
                response_object["error"] = "Invalid request format";
            }
        } else {
            response_object["error"] = "Invalid JSON format";
        }

        // Формирование ответа с пробелами между ключом и значением
        std::ostringstream response_stream;
        response_stream << "{ ";
        bool first = true;
        for (const auto& kv : response_object) {
            if (!first)
                response_stream << ", ";
            response_stream << "\"" << kv.key() << "\": " << kv.value();
            first = false;
        }
        response_stream << " }";

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n" +
                               response_stream.str();

        std::cout << "Sending response: " << response << std::endl;
        boost::asio::write(socket, boost::asio::buffer(response), error);
        socket.shutdown(tcp::socket::shutdown_both, error);
    } catch (std::exception& e) {
        std::cerr << "Exception in handle_request: " << e.what() << std::endl;
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
        std::cerr << "Exception in main: " << e.what() << std::endl;
    }
    return 0;
}
