#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/descriptor.h"

#define FIFO_PATH "/tmp/fifo_file"
#define BUFFER_SIZE 128

int main() {
    while (access(FIFO_PATH, F_OK) == -1) {
        printf("Waiting to start server...\n");
        sleep(1);
    }
    printf("Connected to server. Starting automatic guess...\n");
    file_descriptor_t fds_write;
    file_descriptor_t fds_read;
    int low = 1;
    int high = 100;
    int guess;
    while (1) {
        guess = (low + high) / 2;
        printf("Sent: %d\n", guess);
        char guess_str[BUFFER_SIZE];
        snprintf(guess_str, BUFFER_SIZE, "%d", guess);
        file_descriptor_t *ptr_fds_write = &fds_write;
        open_fds_connection(ptr_fds_write, FIFO_PATH, O_WRONLY);
        write_bytes_to_file(ptr_fds_write, guess_str);
        close_fds_connection(ptr_fds_write);
        file_descriptor_t *ptr_fds_read = &fds_read;
        open_fds_connection(ptr_fds_read, FIFO_PATH, O_RDONLY);
        char response[BUFFER_SIZE];
        ssize_t msg_size = read_bytes_from_file(ptr_fds_read, response);
        response[msg_size] = '\0';
        close_fds_connection(ptr_fds_read);
        printf("Received: %s\n\n", response);
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
