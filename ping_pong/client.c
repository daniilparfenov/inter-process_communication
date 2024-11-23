#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inter-process-communication.h"

#define FIFO_PATH "/tmp/fifo"
#define BUFFER_SIZE 128

// Обработчик сигнала
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\n[Error] Received SIGINT. Exiting...\n");
        exit(0);
    }
    if (sig == SIGPIPE) {
        printf("\n[Error] Received SIGPIPE.\n");
    }
}

// Возвращает кол-во запросов для сервера, которое парсится из флагов для
// программы -1 в случае, если нужно делать бесконечное количество запросов
int get_queries_number_from_console_flag(int argc, char* argv[]) {
    int queries_number = 5;
    int opt;

    // Парсинг флагов
    while ((opt = getopt(argc, argv, "n:e")) != -1) {
        switch (opt) {
            case 'n':
                queries_number = atoi(optarg);  // Преобразуем строку в int
                if (queries_number <= 0) {
                    fprintf(stderr, "Invalid -n flag value: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'e':
                queries_number = -1;
                break;
            case '?':  // Некорректная опция
                fprintf(stderr, "Invalid flag\n");
                exit(1);
        }
    }
    return queries_number;
}

int main(int argc, char* argv[]) {
    // Установка обработчика сигнала SIGINT
    signal(SIGINT, handle_signal);

    // Установка обработчика сигнала SIGPIPE
    signal(SIGPIPE, handle_signal);

    const char* message = "pong";
    char buffer[BUFFER_SIZE];
    int query_number = get_queries_number_from_console_flag(argc, argv);

    printf("Client starting...\n");
    start_client(FIFO_PATH);

    while (1) {
        // Попытка получить сообщение от сервера
        if (receive_message(buffer, BUFFER_SIZE, FIFO_PATH) < 0) {
            printf("Receive message error\n");
            printf("The connection to the server has been terminated maybe\n");
            break;
        } else {
            if (strcmp(buffer, "ping") != 0) {
                printf("Wrong answer from server received: %s\n", buffer);
            } else {
                printf("Correct answer from server received: %s\n", buffer);
                send_message(message, FIFO_PATH);
                printf("Sent message: %s\n", message);
            }
        }
        sleep(2);

        if (query_number != -1) {
            if (--query_number == 0) {
                break;
            }
        }
    }
    return 0;
}
