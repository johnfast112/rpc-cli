#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try{ //Handle the cli options
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << "rpc-cli: " << x.what() << '\n';
    std::cerr << "usage: rpc-cli [-c|--connect [-f|--file <file>]] [-s|--server [-f|--file <file>]]\n";
    return 1;
  }

  //Print help
  if(program_options::help()){
    //TODO:
  }

  RPC rpc{};

    try {
      rpc.init();
    } catch (const std::exception &x) {
      std::cerr << "rpc-cli: " << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-c|--connect [-f|--file <file>]]\n";
      return 1;
    }

  if(program_options::online()){
    rpc.n_run();
  } else {
    rpc.l_run();
  }

  return 0;
}
