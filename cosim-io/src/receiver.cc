#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"
#include "commit_log_pack.h"

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

  int bind_status = bind(server_sock_fd, (sockaddr *)&server_addr, sizeof(server_addr));
  perror("Bind");
  if (bind_status < 0)
  {
    close(server_sock_fd);
    return 1;
  }

  int listen_status = listen(server_sock_fd, BACKLOG);
  perror("Listen");
  if (listen_status)
  {
    close(server_sock_fd);
    return 1;
  }

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  client_sock_fd = accept(server_sock_fd, (sockaddr *)&client_addr, &client_len);
  perror("Accept");
  if (client_sock_fd < 0)
  {
    close(server_sock_fd);
    return 1;
  }

  while (true)
  {
    if (!simulation_completed())
    {
      // for (size_t i = 0; i < 100; i++)
      step(); // spike simulasyon step
      const int MAX_COMMIT_LOG_LENGTH = 200;
      uint8_t generated_buffer[MAX_COMMIT_LOG_LENGTH];
      uint8_t received_buffer[MAX_COMMIT_LOG_LENGTH];
      auto state = s_ptr->get_core(0)->get_state();
      size_t bytes_written = pack_commit_log_into_array(generated_buffer, MAX_COMMIT_LOG_LENGTH, *state);

      // reading bytes_written bytes. 
      // assuming other end of the simulation 
      // writes equal number of bytes for the current step
      // receive it and compare with the 
      int recv_status = recv(client_sock_fd, &received_buffer, bytes_written, 0);
      if (recv_status < 0)
      {
        perror("Receive failed");
        close(client_sock_fd);
        close(server_sock_fd);
        return 1;
      }
      if (memcmp(received_buffer,generated_buffer,bytes_written) != 0)
      {
        // fprintf(stderr, "buffers dosern't match\n");
      }
    }
    else
    {
      break;
    }
  }
}
