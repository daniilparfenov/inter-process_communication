#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "descriptor.h"

#define FIFO_PATH "/tmp/fifo_file"
#define BUFFER_SIZE 128

// Обработчик сигнала
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived SIGINT. Cleaning up...\n");
        unlink(FIFO_PATH);
        exit(0);
    }
}

int main() {
    // Установка обработчика сигнала SIGINT
    signal(SIGINT, handle_signal);

    srand(time(NULL));

    // Создание FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            handle_error("mkfifo");
        }
    }

    printf("Server started. Waiting for client...\n");

    // Создаем дескриптор для чтения
    file_descriptor_t fds_read;
    file_descriptor_t *ptr_fds_read = &fds_read;

    // Создаем дескриптор для записи
    file_descriptor_t fds_write;
    file_descriptor_t *ptr_fds_write = &fds_write;

    int guessed_number = rand() % 100 + 1;

    char buffer[BUFFER_SIZE];  // Буффер для обработки текста от пользователя
    size_t game_counter = 1;
    size_t game_attempt = 0;

    printf("\nGame number %zu\n\n", game_counter);

    while (1) {
        // Открытие FIFO на чтение
        open_fds_connection(ptr_fds_read, FIFO_PATH, O_RDONLY);
        ++game_attempt;

        // Чтение сообщения от клиента
        ssize_t msg_size = read_bytes_from_file(ptr_fds_read, buffer);
        close_fds_connection(ptr_fds_read);

        buffer[msg_size] = '\0';  // Нулевой символ в конце строки
        int estimated = atoi(buffer);
        printf("[Attempt %zu] << Received: %d\n", game_attempt, estimated);

        // Ответ клиенту
        open_fds_connection(ptr_fds_write, FIFO_PATH, O_WRONLY);

        // Анализ ответа пользователя
        const char *msg = "\n";
        if (estimated == guessed_number) {
            msg = "You guessed the number!";
            ++game_counter;
        } else if (estimated > guessed_number) {
            msg = "The guessed number is less";
        } else {
            msg = "The guessed number is more";
        }

        sleep(1);

        // Отправка ответа пользователю
        write_bytes_to_file(ptr_fds_write, msg);
        close_fds_connection(ptr_fds_write);
        printf("[Attempt %zu] >> Sent: %s\n\n", game_attempt, msg);
        if (estimated == guessed_number) {
            printf("Game number %zu\n\n", game_counter);
            game_attempt = 0;
        }
    }

    unlink(FIFO_PATH);  // Удаление FIFO
    return 0;
}
