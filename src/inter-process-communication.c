#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int start_server(const char* fifo_path) {
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}

void start_client(const char* fifo_path) {
    while (access(fifo_path, F_OK) == -1) {
        printf("Waiting to start server...\n");
        sleep(1);
    }
    printf("Connected to server successfully.\n");
}

ssize_t send_message(const char* message, const char* fifo_path) {
    int fd_writer = open(fifo_path, O_WRONLY);
    if (fd_writer < 0) return -1;
    ssize_t written_bytes = write(fd_writer, message, strlen(message) + 1);
    close(fd_writer);
    return written_bytes;
}

ssize_t receive_message(char* buffer, const int buffer_size, const char* fifo_path) {
    int fd_reader = open(fifo_path, O_RDONLY);
    if (fd_reader < 0) return -1;
    ssize_t read_bytes = read(fd_reader, buffer, buffer_size);
    close(fd_reader);
    return read_bytes;
}
