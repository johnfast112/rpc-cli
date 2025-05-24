#include "program_options.h"

//This stuff was stoled straight off of medium.com
//Don't know if this type of behaviour is allowed but here's what the link was as of May 22, 2025:
//https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f

namespace {
static bool         _broadcast{ false };
static bool         _connect  { false };
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
      if(_connect){
        throw std::runtime_error("Cannot use -c/--connect option twice!");
      }
      if(_broadcast){
        throw std::runtime_error("Cannot use -c/--connect option with -b/--broadcast");
      }
      _connect = true;
      continue;
    }

    if(*arg == "-b" || *arg == "--broadcast"){
      if(_broadcast){
        throw std::runtime_error("Cannot use -b/--broadcast connect twice!");
      }
      if(_connect){
        throw std::runtime_error("Cannot use -b/--broadcast option with -c/--connect");
      }
      _broadcast = true;
      continue;
    }

    if(*arg == "-f" || *arg == "--file"){
      if(_fileopt){
        throw std::runtime_error("Cannot use -f/--file param twice!");
      }
      if(!_broadcast && !_connect){
        throw std::runtime_error("Cannot use -f/--file without first specifying -c/--connect or -b/--broadcast");
      }

      if(arg + 1 != end){
        _fileopt = true;
        _file = *(arg + 1);
        continue;
      }
    }
  }
}

bool program_options::broadcast(){
  return _broadcast;
}

bool program_options::connect(){
  return _connect;
}

bool program_options::fileopt(){
  return _fileopt;
}

const std::string_view& program_options::file(){
  return _file;
}
