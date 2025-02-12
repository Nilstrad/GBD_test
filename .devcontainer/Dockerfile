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
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем рабочую директорию
WORKDIR /app
