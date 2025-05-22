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
  rpc.getA();
  rpc.getB();
  rpc.print();

  if(program_options::online()){
    std::cout << "ONLINE\n";
  }

  if(program_options::fileopt()){
    std::ifstream f(static_cast<std::string>(program_options::file()), std::ios::in);
    if(!f.is_open()){
      std::cerr << "rpc-cli: Could not open input file " << program_options::file() << '\n';
      return 2;
    }
    std::string line;
    while(std::getline(f, line)){
      std::cout << line << '\n';
    }
  }

  return 0;
}
