1. Создать C++ Hello World проект: main.cpp
2. Создать CMakeList.txt.

Например:

    cmake_minimum_required(VERSION 3.10)

    project(HelloWorld)

    set(CMAKE_CXX_STANDARD 17)

    add_executable(HelloWorld main.cpp)


3. Скомпилировать и запусить с помощью команд. 

Команды:

    mkdir build

    cd build

    cmake ..

    make

    ./HelloWorld
4. Установить Docker.(Скачать и установить DockerDesktop).
5. Создать Docker образ.(Dockerfile):

Например:
    FROM ubuntu:latest

    RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    git \
    curl \
    unzip \
    && rm -rf /var/lib/apt/lists/*

    WORKDIR /app
6. Запускаем DockerDesktop, в консоли создаем образ, затем запускаем контейнер.

Например:

    docker build -t cpp-dev .
    docker run -it -v C:\path\to\your\project:/workspace --name cpp_container cpp-dev

7. Потом заходим в vs code с расширением Dev Containers. И подключаемся к нашему активному контейнеру. И если хотим запустить наш проект, что возвращаемся на шаг 1.
