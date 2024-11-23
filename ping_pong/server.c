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
    printf("\n[Server] Received SIGPIPE.\n");
  }
}

int main() {
  // Установка обработчика сигнала SIGINT
  signal(SIGINT, handle_signal);

  // Установка обработчика сигнала SIGPIPE
  signal(SIGPIPE, handle_signal);

  const char* message = "ping";
  char buffer[BUFFER_SIZE];
  int iteration_num = 0;

  printf("Starting server...\n");
  if (start_server(FIFO_PATH) < 0) {
    perror("Starting server error\n");
    exit(0);
  }

  printf("The server started succesfully\n");
  printf("Waiting for a client...\n");

  while (1) {
    send_message(message, FIFO_PATH);
    printf("\nIteration #%d\n", iteration_num);
    printf("Sent message: %s\n", message);
    if (receive_message(buffer, BUFFER_SIZE, FIFO_PATH) < 0) {
      printf("receive message error\n");
    } else {
      if (strcmp(buffer, "pong") != 0) {
        printf("Wrong answer from client received: %s\n", buffer);
      } else {
        printf("Correct answer from client received: %s\n", buffer);
      }
    }
    iteration_num++;
  }

  unlink(FIFO_PATH);  // Удаление FIFO
  return 0;
}