#ifndef RPC_H
#define RPC_H

#include "program_options.h"
#include <iostream>
#include <limits>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/select.h>

#include <unistd.h>

#include <cstring> // For memset

constexpr int STDIN{0};

class RPC{
private:
  enum Move : uint16_t{
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3,
    MAX_MOVE = 0,
  };

  int m_sockfd{-1};
  int m_listener{-1};

  fd_set master_fds;
  fd_set read_fds;

  int fd_max;

  Move m_a{MAX_MOVE};
  Move m_b{MAX_MOVE};

  void* get_in_addr(struct sockaddr*);
  int sendall(int s, char *buf, int *len);

  Move get_move();
  void handle_fd(int i);

  void get_a();
  void get_b();

  void c_connect();
  void s_init();

  friend std::ostream& operator<<(std::ostream&, RPC::Move);

public:
  void init();

  void s_accept();

  void n_run();
  void l_run();

  void print();
};

#endif
