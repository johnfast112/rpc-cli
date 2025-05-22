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

class RPC{
private:
  enum Move : uint16_t{
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3,
    MAX_MOVE = 0,
  };

  int m_sockfd;
  int m_listener;

  fd_set master_fds;
  fd_set read_fds;

  int fd_max;

  Move m_A{MAX_MOVE};
  Move m_B{MAX_MOVE};

  Move getMove();
  Move awaitMove();

public:
  void getA();
  void getB();

  void c_connect();
  void startServer();
  void s_listen();

  void print();
};

#endif
