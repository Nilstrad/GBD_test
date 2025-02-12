# Используем официальный образ Ubuntu
FROM ubuntu:latest

# Устанавливаем необходимые инструменты
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    git \
    curl \
    unzip \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем рабочую директорию
WORKDIR /app


# Прокидываем порт для сервера
EXPOSE 8080
