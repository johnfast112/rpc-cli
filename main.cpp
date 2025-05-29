#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try{ //Handle the cli options
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << x.what() << '\n';
    std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
    return 1;
  }

  RPC rpc{};

  if(program_options::server()){
    try {
      rpc.s_init();
      rpc.s_listen();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
      return 1;
    }
  }
  if(program_options::online()){
    try {
      rpc.c_connect();
      rpc.c_run();
    } catch (const std::exception &x) {
      std::cerr << x.what() << '\n';
      std::cerr << "usage: rpc-cli [-o|--online [-f|--file <file>]]\n";
      return 1;
    }
  }

  rpc.get_a();
  rpc.get_b();
  rpc.print();

  std::cout << sizeof(RPC);

  return 0;
}
