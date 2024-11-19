#include "../include/descriptor.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 128

void handle_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void open_fds_connection(file_descriptor_t* fds, const char* path, const int flags) {
    fds->connection_id = open(path, flags);
    if (fds->connection_id < 0) {
        printf("Errno = %d\t function %s\t file %s\t line %d\n", errno, __func__, __FILE__,
               __LINE__);
        handle_error("Error message: ");
    }
}

void close_fds_connection(file_descriptor_t* fds) { close(fds->connection_id); }

ssize_t read_bytes_from_file(file_descriptor_t* fds, void* buff) {
    ssize_t bytes_read = read(fds->connection_id, buff, BUFFER_SIZE - 1);
    if (bytes_read == -1) {
        close(fds->connection_id);
        handle_error("read");
    }
    return bytes_read;
}

ssize_t write_bytes_to_file(file_descriptor_t* fds, const void* buff) {
    ssize_t bytes_write = write(fds->connection_id, buff, strlen(buff));
    return bytes_write;
}
