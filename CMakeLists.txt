CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


# add_compile_options(-g)

PROJECT(EchoServer_Epoll)

AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(echo_server ${SRC_LIST})

# target_link_libraries(echo_server unp)


