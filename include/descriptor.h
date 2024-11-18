#ifndef INCLUDE_DESCRIPTOR_H_
#define INCLUDE_DESCRIPTOR_H_

#include <unistd.h>

typedef struct file_descriptor {
    int connection_id;
} file_descriptor_t;

void handle_error(const char* msg);

void open_fds_connection(file_descriptor_t* fds, const char* path, const int flags);

void close_fds_connection(file_descriptor_t* fds);

ssize_t read_bytes_from_file(file_descriptor_t* fds, void* buff);

ssize_t write_bytes_to_file(file_descriptor_t* fds, const void* buff);

#endif  // INCLUDE_DESCRIPTOR_H_
