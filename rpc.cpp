#include "rpc.h"

void RPC::getA(){
  std::cout << "What\'s Your Move? (1/2/3)\n";
  std::cout << "1) Rock\n2) Paper\n3) Scissors\n";

  char c;
  while(true){
    std::cout << "Try: ";
    std::cin >> c;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(std::cin.good()){
      break;
    }
  }

  switch(c){
    case '1':
      m_A=ROCK;
      break;
    case '2':
      m_A=PAPER;
      break;
    case '3':
      m_A=SCISSORS;
      break;
    default:
      m_A=MAX_MOVE;
  }
}

void RPC::getB(){
  m_B=SCISSORS;
}

void RPC::print(){
  if(m_A == MAX_MOVE || m_B == MAX_MOVE){
    std::cout << "A Player Has Yet To Make Their Move\n";
    return;
  }

  if(m_A == m_B){
    std::cout << "TIE!\n";
    return;
  }

  bool aWins;
  switch(m_A){
    case ROCK:
      switch(m_B){
        case PAPER:
          aWins=false;
          break;
        case SCISSORS:
          aWins=true;
          break;
      }
      break;
    case PAPER:
      switch(m_B){
        case ROCK:
          aWins=true;
          break;
        case SCISSORS:
          aWins=false;
          break;
      }
      break;
    case SCISSORS:
      switch(m_B){
        case PAPER:
          aWins=true;
          break;
        case ROCK:
          aWins=false;
          break;
      }
      break;
    default:
      std::cerr << "WE SHOULDN'T BE HERE AAAH!\n";
      return;
  }

  std::cout << "The Winner Is: " << (aWins ? "A" : "B") << '\n';
}
