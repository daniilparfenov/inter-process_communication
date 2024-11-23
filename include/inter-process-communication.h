#ifndef INCLUDE_INTER_PROCESS_COMMUNICATION_H_
#define INCLUDE_INTER_PROCESS_COMMUNICATION_H_

#include <sys/types.h>

int start_server(const char* fifo_path);
void start_client(const char* fifo_path);

ssize_t send_message(const char* message, const char* fifo_path);
ssize_t receive_message(char* buffer, const int buffer_size, const char* fifo_path);

#endif  // INCLUDE_INTER_PROCESS_COMMUNICATION_H_