#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try { //Setup bools for cli options
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << x.what() << '\n';
    std::cerr << "usage: rpc-cli [-c|--connect] [-b|--broadcast] [-f|--file [file]]\n";
    return 1;
  }

  RPC rpc{};

  if(program_options::broadcast()){
    try {
      rpc.broadcast();
    } catch (const std::exception& x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-c|--connect] [-b|--broadcast] [-f|--file [file]]\n";
      return 1;
    }
  }
  if(program_options::connect()){
    try {
      rpc.c_connect();
    } catch (const std::exception& x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-c|--connect] [-b|--broadcast] [-f|--file [file]]\n";
      return 1;
    }
  }

  rpc.run();

  rpc.print();

  return 0;
}
