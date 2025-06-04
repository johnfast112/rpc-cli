#include "program_options.h"

//This stuff was stoled straight off of medium.com
//Don't know if this type of behaviour is allowed but here's what the link was as of May 22, 2025:
//https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f

namespace {
static bool         _help     { false };
static bool         _server   { false };
static bool         _client   { false };
static bool         _fileopt  { false };
static std::string_view _file {};
}

void program_options::parse(int argc, char* argv[]){
  //Avoid getting more than INT_MAX inputs because everything is scary
  if(argc > 64){
    throw std::runtime_error("too many input parameters!");
  }

  //Makes bound checking and iterator arithmetic easier
  const std::vector<std::string_view> args(argv, argv + argc);

  for(auto arg = args.begin()+1, end = args.end(); arg != end; ++arg){
    //Set _client if appropriate
    if(*arg == "-c" || *arg == "--connect"){
      if(_client){
        throw std::runtime_error("Cannot use -c/--connect option twice!");
      }
      if(_server){
        throw std::runtime_error("Cannot use -c/--connect with -s/--server");
      }
      _client = true;
      continue;
    }

    //Set _server if appropriate
    if(*arg == "-s" || *arg == "--server"){
      if(_server){
        throw std::runtime_error("Cannot use -s/--server option twice!");
      }
      if(_client){
        throw std::runtime_error("Cannot use -s/--server with -c/--connect");
      }
      _server = true;
      continue;
    }

    //Set _fileopt and _file if possible, otherwise exit with error
    if(*arg == "-f" || *arg == "--file"){
      if(_fileopt){
        throw std::runtime_error("Cannot use -f/--file option twice!");
      }
      if(!_client && !_server){
        throw std::runtime_error("Cannot use -f/--file without first specifying -c/--connect or -s/--server");
      }

      //Try to grab the filename from the next parameter: --file <file>
      if(arg + 1 != end){
        _fileopt = true;
        _file = *(arg + 1);
        ++arg; //Dont read arg as option
        continue;
      }
      throw std::runtime_error("Option -f/--file needs an argument!");
    }

    //Just exit without warning. No need for a cleaner solution
    if(*arg == "-h" || *arg == "--help"){
      _help = true;
      continue;
    }

    if(
      *arg != "-h" &&
      *arg != "--help" &&
      *arg != "-c" &&
      *arg != "--connect" &&
      *arg != "-f" &&
      *arg != "--file" &&
      *arg != "-s" &&
      *arg != "--server"
    ){
      throw std::runtime_error("Invalid option '" + static_cast<std::string>(*arg) + '\'');
    }
  }
}

bool program_options::help(){
  return _help;
}

bool program_options::server(){
  return _server;
}

bool program_options::client(){
  return _client;
}

bool program_options::online(){
  return _client || _server;
}

bool program_options::fileopt(){
  return _fileopt;
}

const std::string_view& program_options::file(){
  return _file;
}
