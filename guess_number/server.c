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

int main() {
    srand(time(NULL));
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            handle_error("mkfifo");
        }
    }

    printf("Server started. Waiting for client...\n");

    file_descriptor_t fds_read;
    file_descriptor_t *ptr_fds_read = &fds_read;

    file_descriptor_t fds_write;
    file_descriptor_t *ptr_fds_write = &fds_write;

    int guessed_number = rand() % 100 + 1;

    char buffer[BUFFER_SIZE];
    size_t game_counter = 1;
    size_t game_attempt = 0;

    printf("\nGame number %zu\n\n", game_counter);

    while (1) {
        open_fds_connection(ptr_fds_read, FIFO_PATH, O_RDONLY);
        ++game_attempt;

        ssize_t msg_size = read_bytes_from_file(ptr_fds_read, buffer);
        close_fds_connection(ptr_fds_read);

        buffer[msg_size] = '\0';
        int estimated = atoi(buffer);
        printf("[Attempt %zu] << Received: %d\n", game_attempt, estimated);

        open_fds_connection(ptr_fds_write, FIFO_PATH, O_WRONLY);

        const char *msg = "\n";
        if (estimated == guessed_number) {
            msg = "You guessed the number!";
            ++game_counter;
        } else if (estimated > guessed_number) {
            msg = "The guessed number is less";
        } else {
            msg = "The guessed number is more";
        }

        sleep(3);

        write_bytes_to_file(ptr_fds_write, msg);
        close_fds_connection(ptr_fds_write);
        printf("[Attempt %zu] >> Sent: %s\n\n", game_attempt, msg);
        if (estimated == guessed_number) {
            printf("Game number %zu\n\n", game_counter);
            game_attempt = 0;
        }
    }

    unlink(FIFO_PATH);
    return 0;
}
