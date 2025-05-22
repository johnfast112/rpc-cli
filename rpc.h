#ifndef RPC_H
#define RPC_H

#include "program_options.h"
#include <iostream>
#include <limits>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

#include <cstring> // For memset

class RPC{
private:
  enum Move{
    ROCK,
    PAPER,
    SCISSORS,
    MAX_MOVE,
  };

  int m_sockfd;

  Move m_A{MAX_MOVE};
  Move m_B{MAX_MOVE};

  Move getMove();
  Move awaitMove();

public:
  void getA();
  void getB();

  void getConnection();
  void startServer();

  void print();
};

#endif
