CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


add_compile_options(-g3)

PROJECT(EchoServer_Epoll)

# AUX_SOURCE_DIRECTORY(. SRC_LIST)
set(AUTO_BUFFER_ECHO_SERVER ./utils.c ./socket_utils.c ./auto_buffer.c ./agent.c ./tcpservepollnonbET_RDHUB.c)

# ADD_EXECUTABLE(auto_buffer_echo_server ${SRC_LIST})
ADD_EXECUTABLE(auto_buffer_echo_server ${AUTO_BUFFER_ECHO_SERVER})

# target_link_libraries(echo_server unp)


