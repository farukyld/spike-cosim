#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"

#define DEST_IP "127.0.0.1"
#define DEST_PORT 12345
#define BUF_SIZE 8

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
      // for (size_t i = 0; i < 100; i++)
      step(); // spike simulasyon step
      reg_t pc = s_ptr->get_core(0)->get_state()->pc;
      // s_ptr->get_core(0)->get_state()->log_reg_write;
      // s_ptr->get_core(0)->get_state()->log_mem_write;
      // Send the TCP message
      if (send(sock_fd, &pc, sizeof(reg_t), 0) < 0)
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
