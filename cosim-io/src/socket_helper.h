#ifndef SOCKET_HELPER_H
#define SOKET_HELPER_H
#include <stddef.h>
#include <sys/types.h>
ssize_t recv_exactly_n_bytes(int sockfd, void *buf, size_t n, int flags);
#endif // SOCKET_HELPER_H