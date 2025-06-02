#include "program_options.h"

//This stuff was stoled straight off of medium.com
//Don't know if this type of behaviour is allowed but here's what the link was as of May 22, 2025:
//https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f

namespace {
static bool         _server   { false };
static bool         _client   { false };
static bool         _fileopt  { false };
static std::string_view _file {};
}

void program_options::parse(int argc, char* argv[]){
  if(argc > 64){
    throw std::runtime_error("too many input parameters!");
  }

  const std::vector<std::string_view> args(argv, argv + argc);

  for(auto arg = args.begin(), end = args.end(); arg != end; ++arg){
    if(*arg == "-c" || *arg == "--connect"){
      if(_client){
        throw std::runtime_error("Cannot use -c/--connect param twice!");
      }
      if(_server){
        throw std::runtime_error("Cannot use -c/--connect with -s/--server");
      }
      _client = true;
      continue;
    }

    if(*arg == "-f" || *arg == "--file"){
      if(_fileopt){
        throw std::runtime_error("Cannot use -f/--file param twice!");
      }
      if(!_client && !_server){
        throw std::runtime_error("Cannot use -f/--file without first specifying -c/--connect or -s/--server");
      }

      if(arg + 1 != end){
        _fileopt = true;
        _file = *(arg + 1);
        continue;
      }
      throw std::runtime_error("Option -f/--file needs an argument!");
    }

    if(*arg == "-s" || *arg == "--server"){
      if(_server){
        throw std::runtime_error("Cannot use -s/--server param twice!");
      }
      if(_client){
        throw std::runtime_error("Cannot use -s/--server with -c/--connect");
      }
      _server = true;
      continue;
    }

    if(*arg == "-h" || *arg == "--help"){
      throw std::runtime_error("");
      continue;
    }

  }

}

bool program_options::server(){
  return _server;
}

bool program_options::client(){
  return _client;
}

bool program_options::fileopt(){
  return _fileopt;
}

const std::string_view& program_options::file(){
  return _file;
}
