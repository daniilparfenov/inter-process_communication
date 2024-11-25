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
        printf("\n[Server] Received SIGINT.\n");
        unlink(FIFO_PATH);
        exit(0);
    }
    if (sig == SIGPIPE) {
        printf("\n[Server] Сonnection with the client is lost.\n");
    }
}

int main() {
    // Установка обработчика сигнала SIGINT
    signal(SIGINT, handle_signal);

    // Установка обработчика сигнала SIGPIPE
    signal(SIGPIPE, handle_signal);

    const char* message = "pong";
    char buffer[BUFFER_SIZE];
    int cur_query_num = 0;

    printf("Starting server...\n");
    if (start_server(FIFO_PATH) < 0) {
        perror("Starting server error\n");
        exit(0);
    }

    printf("The server started succesfully\n");
    printf("Waiting for a client...\n");

    while (1) {
        printf("\n");
        if (receive_message(buffer, BUFFER_SIZE, FIFO_PATH) < 0) {
            printf("receive message error\n");
        } else {
            cur_query_num++;
            printf("Current query number: %d\n", cur_query_num);
            if (strcmp(buffer, "ping") != 0) {
                printf("Wrong query from client received: %s\n", buffer);
            } else {
                printf("Correct query from client received: %s\n", buffer);
            }

            if (send_message(message, FIFO_PATH) < 0) {
                printf("Sending response error\n");
                printf("Note: the client did not read the response maybe\n");
            } else {
                printf("Sent answer message: %s\n", message);
            }
        }
    }

    unlink(FIFO_PATH);  // Удаление FIFO
    return 0;
}
