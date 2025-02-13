cmake_minimum_required(VERSION 3.10)
project(HttpEchoServer)

set(CMAKE_CXX_STANDARD 17)

# Ищем библиотеку Boost
find_package(Boost REQUIRED COMPONENTS system thread json)

# Добавляем исполняемый файл
add_executable(calculator_server calculator_server.cpp)
add_executable(client cli_client.cpp)

# Линкуем с библиотеками Boost
target_link_libraries(calculator_server Boost::system Boost::thread Boost::json)
target_link_libraries(client Boost::system Boost::thread Boost::json)
