#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try {
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << x.what() << '\n';
    std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
    return 1;
  }

  RPC rpc{};

  if(program_options::server()){
    try {
      rpc.startServer();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
      return 1;
    }
  }
  if(program_options::online()){
    try {
      rpc.c_connect();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
      return 1;
    }
  }

  rpc.getA();
  rpc.getB();
  rpc.print();

  return 0;
}
