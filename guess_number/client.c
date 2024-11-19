#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "descriptor.h"

#define FIFO_PATH "/tmp/fifo_file"
#define BUFFER_SIZE 128

// Обработчик сигнала
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived SIGINT\n");
        exit(0);
    }
}

int main() {
    // Установка обработчика сигнала SIGINT
    signal(SIGINT, handle_signal);

    // Ожидание создания FIFO
    while (access(FIFO_PATH, F_OK) == -1) {
        printf("Waiting to start server...\n");
        sleep(1);
    }

    printf("Connected to server. Starting automatic guess...\n");

    // Создаем дескриптор для записи
    file_descriptor_t fds_write;
    file_descriptor_t *ptr_fds_write = &fds_write;

    // Создаем дескриптор для чтения
    file_descriptor_t fds_read;
    file_descriptor_t *ptr_fds_read = &fds_read;

    int low = 1;     // Нижняя граница диапазона
    int high = 100;  // Верхняя граница диапазона
    int guess;       // Текущее предположение

    while (1) {
        guess = (low + high) / 2;  // Определяем новое предположение (середина диапазона)
        printf("Sent: %d\n", guess);

        // Преобразование числа в строку для отправки
        char guess_str[BUFFER_SIZE];
        snprintf(guess_str, BUFFER_SIZE, "%d", guess);

        // Отправка числа серверу
        open_fds_connection(ptr_fds_write, FIFO_PATH, O_WRONLY);
        write_bytes_to_file(ptr_fds_write, guess_str);
        close_fds_connection(ptr_fds_write);

        // Получение ответа от сервера
        open_fds_connection(ptr_fds_read, FIFO_PATH, O_RDONLY);

        // Запись ответа от сервера в response
        char response[BUFFER_SIZE];
        ssize_t msg_size = read_bytes_from_file(ptr_fds_read, response);

        // Сервер закрыл соединение
        if (msg_size == 0) {
            printf("Error: Server has closed the connection. Exiting...\n");
            close_fds_connection(ptr_fds_read);
            break;
        }

        response[msg_size] = '\0';
        close_fds_connection(ptr_fds_read);
        printf("Received: %s\n\n", response);

        // Анализ ответа сервера
        if (strcmp(response, "You guessed the number!") == 0) {
            printf("Client won! The number is %d.\n", guess);
            break;
        } else if (strcmp(response, "The guessed number is more") == 0) {
            low = guess + 1;
        } else if (strcmp(response, "The guessed number is less") == 0) {
            high = guess - 1;
        }
        if (low > high) {
            printf("Error: Something went wrong. No numbers left to guess.\n");
            break;
        }
    }

    return 0;
}
