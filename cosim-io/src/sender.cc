#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"
#include "commit_log_pack.h"

#define DEST_IP "127.0.0.1"
#define DEST_PORT 12345
#define BUF_SIZE 8

// sender fpga'de calisan islemciyi temsil ediyor
int main()
{
  init(); // spike simulasyon init

  int sock_fd;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  perror("Socket creation");
  if (sock_fd < 1)
  {
    return 1;
  }

  sockaddr_in server_addr;

  // Configure the server address (destination device)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(DEST_PORT);
  server_addr.sin_addr.s_addr = inet_addr(DEST_IP);

  // Connect to the server (target device)
  int connect_stat = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  perror("Connection");
  if (connect_stat < 0)
  {
    close(sock_fd);
    return 1;
  }

  while (true)
  {
    if (!simulation_completed())
    {
      step(); // spike simulasyon step
      uint8_t buffer[ASSUMED_MAX_COMMIT_LOG_SIZE];
      auto state = s_ptr->get_core(0)->get_state();
      size_t bytes_written = pack_commit_log_into_array(buffer, ASSUMED_MAX_COMMIT_LOG_SIZE, *state);

      if (send(sock_fd, &buffer, bytes_written, 0) < 0)
      {
        perror("Send failed");
        close(sock_fd);
        return 1;
      }
    }
    else
    {
      break;
    }
  }
}
