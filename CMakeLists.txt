CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


add_compile_options(-g3)
# add_compile_options(-lunp)

PROJECT(Thread_EchoServer_Epoll)

AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(thread_echo_server ${SRC_LIST})

target_link_libraries(thread_echo_server pthread)


