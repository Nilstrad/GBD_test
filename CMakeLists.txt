cmake_minimum_required(VERSION 3.10)
project(HttpEchoServer)

set(CMAKE_CXX_STANDARD 17)

# Ищем библиотеку Boost
find_package(Boost REQUIRED COMPONENTS system thread)

# Добавляем исполняемый файл
add_executable(http_echo_server http_echo_server.cpp)

# Линкуем с библиотеками Boost
target_link_libraries(http_echo_server Boost::system Boost::thread)
