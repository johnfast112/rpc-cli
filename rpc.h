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
private:
  enum Move : uint16_t{
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3,
    MAX_MOVE = 0,
  };

  enum State{
    DEFAULT,
    SECTION,
    KEY,
    VALUE,
    COMMENT,
  };
  
  int m_sockfd;

  Move m_A{MAX_MOVE};
  Move m_B{MAX_MOVE};

  void* get_in_addr(struct sockaddr*);

  Move getMove();

  void parseINI();

public:
  void broadcast();
  void connect();
  //void startServer();
  //void s_listen();

  void print();
};

#endif
