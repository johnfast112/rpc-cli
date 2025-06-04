#include "program_options.h"
#include "rpc.h"
#include <iostream>

int main(int argc, char* argv[]){
  try{ //Handle the cli options
    program_options::parse(argc, argv);
  } catch (const std::exception &x) {
    std::cerr << "rpc-cli: " << x.what() << '\n';
    std::cerr << "Try 'rpc-cli --help' for more information\n";
    return 1;
  }

  //Print help
  if(program_options::help()){
    std::cout << "Usage: rpc-cli [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -s, --server                Open a port for another rpc-cli client to connect to\n";
    std::cout << "  -c, --connect               Asks for the address of another rpc-cli client to connect to\n";
    std::cout << "  -f, --file <file>           Reads the provided file for a port number or server address. Must be used with -c or -s options.\n";
    std::cout << "  -h, --help                  Prints this help message\n";
    return 0;
  }

  RPC rpc{};

  try {
    rpc.init();
  } catch (const std::exception &x) {
    std::cerr << x.what() << '\n';
    std::cerr << "Try 'rpc-cli --help' for more information\n";
    return 1;
  }

  if(program_options::online()){
    try{
      rpc.n_run();
    } catch (const std::exception &x) {
      std::cerr << "Opponent left the game\n";
    }
  } else {
    try{
      rpc.l_run();
    } catch (const std::exception &x){
      std::cerr << x.what() <<'\n';
      std::cout << "Something went really wrong with this local game\n"; //TODO This means something aweful must've happened
    }
  }

  return 0;
}
