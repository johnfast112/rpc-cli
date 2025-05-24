#ifndef RPC_H
#define RPC_H

#include "program_options.h"

#include <iostream>
#include <limits>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class RPC{
public:
  enum Move : uint16_t{
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3,
    MAX_MOVE = 0,
  };

private:
  enum State{
    DEFAULT,
    SECTION,
    KEY,
    VALUE,
    COMMENT,
  };

  int m_sockfd;

  Move m_p1{MAX_MOVE};
  Move m_p2{MAX_MOVE};

  void* get_in_addr(struct sockaddr*);

  Move getMove();

  void parseINI();

public:
  void broadcast();
  void connect();

  void run();

  void print();
};

#endif
