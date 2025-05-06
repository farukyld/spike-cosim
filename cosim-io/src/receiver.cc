#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"
#include "commit_log_pack.h"
#include "print_helper.h"

#define DEST_IP "127.0.0.1"
#define PORT 12345
#define BACKLOG 1

// receiver bilgisayar'da calisan spike'i temsil ediyor.
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

  size_t progress_steps = 0;
  while (true)
  {
    if (!simulation_completed())
    {
      // receive the header
      uint8_t received_commit_log[HEADER_SIZE + ASSUMED_MAX_BODY_SIZE];
      size_t received_header_byte_count = recv(client_sock_fd,
                                               received_commit_log,
                                               HEADER_SIZE,
                                               0);
      if (unlikely(received_header_byte_count != HEADER_SIZE))
      { // bunu burada bu sekilde mi kontrol etmemiz gerekiyor?
        printf("received header byte count (%ld)"
               " is not equal to expected (%ld)\n",
               received_header_byte_count, HEADER_SIZE);
        close(server_sock_fd);
        printf("progress: %ld steps\n", progress_steps);
        return 1;
      }

      // parse the header
      reg_t pc;
      uint16_t interupts_taken;
      uint8_t body_size;
      uint8_t priv;
      uint8_t freg_count;
      uint8_t xreg_count;
      uint8_t csr_count;
      uint8_t mem_count;
      const uint8_t *body_ptr;
      bool unpacking_success = unpack_commit_log_header(received_commit_log,
                                                        received_header_byte_count,
                                                        pc,
                                                        interupts_taken,
                                                        body_size,
                                                        priv,
                                                        freg_count,
                                                        xreg_count,
                                                        csr_count,
                                                        mem_count,
                                                        body_ptr);
      if (unlikely(!unpacking_success))
      {
        printf("unpacking failed\n");
        close(server_sock_fd);
        printf("progress: %ld steps\n", progress_steps);
        return 1;
      }

      // printf("received header:\n");
      // print_sliced_hex(received_commit_log, received_header_byte_count, HEADER_FORMAT);

      // receive the body
      size_t received_body_size = recv(client_sock_fd,
                                       received_commit_log + HEADER_SIZE,
                                       body_size,
                                       0);
      if (unlikely(received_body_size != body_size))
      {
        printf("received_body_size (%ld) doesn't match body_size passed with header (%d)\n", received_body_size, body_size);
        printf("progress: %ld steps\n", progress_steps);
        close(server_sock_fd);
        return 1;
      }
      // printf("received_body_size (%ld) - body_size (%d)\n", received_body_size, body_size);

      // printf("received body:\n");
      // print_sliced_hex(received_commit_log + HEADER_SIZE, body_size, {8,1});
      // put the interupts taken into spike.
      //
      //
      //
      step(); // spike simulasyon step
      progress_steps++;
      // take the commit logs from spike.

      uint8_t generated_commit_log[ASSUMED_MAX_COMMIT_LOG_SIZE];
      auto state = s_ptr->get_core(0)->get_state();
      size_t generated_commit_log_length = pack_commit_log_into_array(generated_commit_log,
                                                                      ASSUMED_MAX_COMMIT_LOG_SIZE,
                                                                      *state);

      // compare them
      if (unlikely(memcmp(generated_commit_log, received_commit_log, generated_commit_log_length) != 0))
      {
        printf("logs do not match\n"
               "generated_commit_log:\n");
        print_sliced_hex(generated_commit_log, generated_commit_log_length, HEADER_FORMAT);
        printf("received_commit_log:\n");
        print_sliced_hex(received_commit_log, HEADER_SIZE + received_body_size, HEADER_FORMAT);
        printf("progress: %ld steps\n", progress_steps);
        close(server_sock_fd);
        return 1;
      }
    }
    else
    {
      break;
    }
  }
}
