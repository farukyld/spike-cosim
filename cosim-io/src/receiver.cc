#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"

#define DEST_IP "127.0.0.1"
#define PORT 12345
#define BACKLOG 1

int main()
{
  init(); // spike simulasyon init

  int server_sock_fd, client_sock_fd;



  server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  perror("Server socket creation");
  if (server_sock_fd < 1)
  {
    return 1;
  }

  sockaddr_in server_addr;

  // Configure the server address (destination device)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  int bind_status = bind(server_sock_fd, (sockaddr *)&server_addr,sizeof(server_addr));
  perror("Bind");
  if(bind_status < 0)
  {
    close(server_sock_fd);
    return 1;
  }

  int listen_status  = listen(server_sock_fd, BACKLOG);
  perror("Listen");
  if (listen_status)
  {
    close(server_sock_fd);
    return 1;
  }

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  client_sock_fd = accept(server_sock_fd, (sockaddr *) &client_addr, &client_len);
  perror("Accept");
  if (client_sock_fd < 0){
    close(server_sock_fd);
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
      reg_t received_pc;
      if (recv(client_sock_fd, &received_pc, sizeof(reg_t), 0) < 0)
      {
        perror("Receive failed");
        close(received_pc);
        return 1;
      }
      if (pc != received_pc)
      {
        fprintf(stderr, "pcs dosern't match\n");
      }
    }
    else
    {
      break;
    }
  }
}
