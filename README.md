# There are three branch
1. master: Echo server with fixed length buffer.
2. mul_thread_ver_dev : Echo server with a simple multi-threaded Implement.
3. auto_thread_ver_dev : Echo server with unfixed length buffer, which means I can read until EWOULDBLOCK.

# ps
The above three implementions all use epoll with ET mode;
I just want to test how big is their performance difference, but the answer is that their performance is similiar.
