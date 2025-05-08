#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cosimif.h"
#include "commit_log_pack.h"
#include "print_helper.h"
#include "socket_helper.h"

#define DEST_IP "127.0.0.1"
#define PORT 12345
#define BACKLOG 1

int main()
{
  int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  perror("Server socket creation");
  if (server_sock_fd < 1)
    return 1;

  sockaddr_in server_addr = {};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Bind");
    close(server_sock_fd);
    return 1;
  }

  if (listen(server_sock_fd, BACKLOG))
  {
    perror("Listen");
    close(server_sock_fd);
    return 1;
  }

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sock_fd = accept(server_sock_fd, (sockaddr *)&client_addr, &client_len);
  perror("Accept");
  if (client_sock_fd < 0)
  {
    close(server_sock_fd);
    return 1;
  }

  size_t progress_steps = 0;
  std::unordered_map<uint32_t, size_t> state_change_pattern_counts;
  std::map<uint32_t, size_t> pc_counts;

  while (true)
  {
    uint8_t received_commit_log[HEADER_SIZE + ASSUMED_MAX_BODY_SIZE];
    size_t received_header_byte_count = recv_exactly_n_bytes(client_sock_fd,
                                                             received_commit_log,
                                                             HEADER_SIZE,
                                                             0);
    if (received_header_byte_count != HEADER_SIZE)
      break;

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
    if (!unpacking_success)
      break;

    size_t received_body_size = recv_exactly_n_bytes(client_sock_fd,
                                                     received_commit_log + HEADER_SIZE,
                                                     body_size,
                                                     0);
    if (received_body_size != body_size)
      break;

    if (xreg_count > 2)
    {
      fprintf(stderr, RED "xreg_count > 2. at step: %ld" DEF_COLOR, progress_steps);
      break;
    }
    uint32_t state_change_key = 0;
    state_change_key = (csr_count << 24) | (freg_count << 16) | (xreg_count << 8) | (mem_count << 0);

    // Increment count for this pattern
    state_change_pattern_counts[state_change_key]++;
    pc_counts[pc]++;
    progress_steps++;
  }

  printf("state Change Pattern Counts (csr, freg, xreg, mem):\n");
  for (const auto &[key, count] : state_change_pattern_counts)
  {
    uint8_t csr = (key >> 24) & 0xFF;
    uint8_t freg = (key >> 16) & 0xFF;
    uint8_t xreg = (key >> 8) & 0xFF;
    uint8_t mem = key & 0xFF;
    printf("(%u, %u, %u, %u): %lu\n",csr, freg, xreg, mem, count);
  }

  // Convert the map to a vector of pairs (PC, count)
  std::vector<std::pair<uint32_t, size_t>> pc_vector(pc_counts.begin(), pc_counts.end());

  // Sort the vector by count (descending order)
  std::sort(pc_vector.begin(), pc_vector.end(), [](const std::pair<uint32_t, size_t> &a, const std::pair<uint32_t, size_t> &b)
            {
              return a.second > b.second; // Sort by count in descending order
            });

  // Print the first 100 PCs (sorted by most hits)
  printf("\nTop 100 most hit PCs:\n");
  size_t printed = 0;
  for (const auto &[pc, count] : pc_vector)
  {
    printf("PC: 0x%08x, Count: %lu\n", pc, count);
    if (++printed >= 100)
      break;
  }

  close(client_sock_fd);
  close(server_sock_fd);

  return 0;
}
