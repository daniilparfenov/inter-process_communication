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

int main() {
  // Установка обработчика сигнала SIGINT
  signal(SIGINT, handle_signal);

  // Установка обработчика сигнала SIGPIPE
  signal(SIGPIPE, handle_signal);

  const char* message = "pong";
  char buffer[BUFFER_SIZE];

  printf("Client starting...\n");
  start_client(FIFO_PATH);

  while (1) {
    // Попытка получить сообщение от сервера
    if (receive_message(buffer, BUFFER_SIZE, FIFO_PATH) < 0) {
      printf("receive message error\n");
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
  }
  return 0;
}