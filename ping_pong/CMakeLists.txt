add_executable(ping_pong_client client.c)
add_executable(ping_pong_server server.c)

target_link_libraries(ping_pong_client PRIVATE inter-process-communication)
target_link_libraries(ping_pong_server PRIVATE inter-process-communication)