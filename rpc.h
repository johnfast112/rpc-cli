#ifndef RPC_H
#define RPC_H

#define PLAYER1 0
#define PLAYER2 1

#define STDIN 1

#include "program_options.h"

#include <arpa/inet.h> //For inet_ntop

#include <cstring> //For memset

#include <iostream>
#include <limits>
#include <stdint.h>
#include <sys/select.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>

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

  int m_listener;
  int m_oppfd;
  int fd_max;

  fd_set master_fds;
  fd_set read_fds;

  Move m_p1{MAX_MOVE};
  Move m_p2{MAX_MOVE};

  void* get_in_addr(struct sockaddr*);

  Move getMove(bool);

  void parseINI();

public:
  void broadcast();
  void c_connect();

  void run();

  void print();
};

#endif
