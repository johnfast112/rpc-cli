#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try{ //Handle the cli options
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << x.what() << '\n';
    std::cerr << "usage: rpc-cli [-c|--connect [-f|--file <file>]] [-s|--server [-f|--file <file>]]\n";
    return 1;
  }

  RPC rpc{};

  if(program_options::server()){
    try {
      rpc.s_init();
      rpc.n_run();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-s|--server [-f|--file <file>]]\n";
      return 1;
    }
  }
  if(program_options::client()){
    try {
      rpc.c_connect();
      rpc.n_run();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-c|--connect [-f|--file <file>]]\n";
      return 1;
    }
  }

  if(!program_options::client() && !program_options::server()){
    rpc.get_a();
    rpc.get_b();
    rpc.print();
  }

  std::cout << sizeof(RPC);

  return 0;
}
