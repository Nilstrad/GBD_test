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
8. 5 уровень.

Ответ получен:

    Echo: GET / HTTP/1.1

    Host: localhost:8080

    User-Agent: curl/8.9.1

    Accept: */*

9. 6 уровень. Добавлен калькулятор.
10. 7 уровень. Добавлен Calculator Server.
    Отправленный запрос выглядит так:

        curl -X POST http://localhost:8080 -d "{\"exp\": \"2+2\"}" -H "Content-Type: application/json"

        curl -X POST http://localhost:8080 -d "{\"cmd\": \"echo\"}" -H "Content-Type: application/json"

    Ответ выглядит так:

        {"res":"4"}

        {"res":"echo"}

11. 8 уровень. Добавлен cli client.

        root@d81badc4279b:/app/build# ./client -e 2+2
        4
        root@d81badc4279b:/app/build# ./client -c echo
        "echo"
    

12. 9 уровень. Добавлена поддержка переменных и многострочного
 ввода.


        root@d81badc4279b:/app/build# ./client -c echo
        "echo"

        root@d81badc4279b:/app/build# ./client -e var = 2 + 5 \
        > var*3
        21

13. 10 уровень. Добавлена поддержка хранения промежуточных результатов.
    
        root@d81badc4279b:/app/build# ./client -e pi=3.14
        3.14
        root@d81badc4279b:/app/build# ./client -e 2*pi *3
        18.84
        root@d81badc4279b:/app/build# ./client -e var = 2*pi*3 \
        > var/3  
        6.28
        root@d81badc4279b:/app/build# ./client -c clean
        "State cleared"
        root@d81badc4279b:/app/build# ./client -e 2*pi
        Ошибка: "Unknown variable 'pi'"

14. 11 уровень. Расширена API для поддержки сессий. Сессия создается и хранится
для каждого пользователя. Пользователь может быть указан при
работе с API калькулятора.
API для сброса состояния сбрасывает состояние только
текущей сессии

        root@d81badc4279b:/app/build# ./client -u student -e pi=3.14
        3.14
        root@d81badc4279b:/app/build# ./client -e 2*pi*3
        Ошибка: "Unknown variable 'pi'"
        root@d81badc4279b:/app/build# ./client -u student -e 2*pi*3
        18.84
        root@d81badc4279b:/app/build# ./client -u stud -e 2*pi*3
        Ошибка: "Unknown variable 'pi'"
        root@d81badc4279b:/app/build# ./client -u student -c clean
        "State cleared"
        root@d81badc4279b:/app/build# ./client -u student -e 2*pi*3
        Ошибка: "Unknown variable 'pi'"