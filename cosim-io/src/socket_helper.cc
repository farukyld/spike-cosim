
#include <stdio.h>
#include <sys/socket.h>

ssize_t recv_exactly_n_bytes(int sockfd, void *buf, size_t n, int flags)
{
  size_t total_bytes_read = 0;
  char *buffer = (char *)buf;

  while (total_bytes_read < n)
  {
    ssize_t bytes_read = recv(sockfd, buffer + total_bytes_read, n - total_bytes_read, flags);
    if (bytes_read == -1)
    {
      perror("recv");
      return -1; // Error during receiving data
    }
    else if (bytes_read == 0)
    {
      // The connection was closed
      fprintf(stderr, "Connection closed by peer\n");
      return -1;
    }

    total_bytes_read += bytes_read;
  }

  return total_bytes_read; // Return the total bytes read
}
