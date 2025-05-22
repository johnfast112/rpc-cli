#ifndef RPC_H
#define RPC_H

#include <iostream>
#include <limits>

class RPC{
private:
  enum Move{
    ROCK,
    PAPER,
    SCISSORS,
    MAX_MOVE,
  };

  Move m_A{MAX_MOVE};
  Move m_B{MAX_MOVE};

public:
  void getA();
  void getB();

  void print();
};

#endif
