#include <iostream>
#include <stack>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cctype>

// Функция для выполнения операций
double apply_operator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw std::invalid_argument("Division by zero");
            return a / b;
        default: throw std::invalid_argument("Unknown operator");
    }
}

// Функция для обработки выражения в польской нотации
double evaluate_rpn(const std::string &expression) {
    std::stack<double> stack;
    std::stringstream ss(expression);
    std::string token;

    while (ss >> token) {
        if (isdigit(token[0])) {
            // Если это число, преобразуем его в double и добавляем в стек
            stack.push(std::stod(token));
        } else if (token.size() == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/')) {
            // Если это оператор, извлекаем два элемента из стека и применяем операцию
            if (stack.size() < 2) {
                throw std::invalid_argument("Insufficient operands");
            }
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            stack.push(apply_operator(a, b, token[0]));
        } else {
            throw std::invalid_argument("Invalid token in expression");
        }
    }

    // Результат должен быть единственным значением в стеке
    if (stack.size() != 1) {
        throw std::invalid_argument("Invalid expression");
    }
    return stack.top();
}

int main() {
    std::string expression;

    std::cout << "Enter an expression in Polish notation (e.g. '3 4 + 2 * 7 /'): ";
    std::getline(std::cin, expression);

    try {
        double result = evaluate_rpn(expression);
        std::cout << "Result: " << result << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
