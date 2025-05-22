#include "program_options.h"

//This stuff was stoled straight off of medium.com
//Don't know if this type of behaviour is allowed but here's what the link was as of May 22, 2025:
//https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f

namespace {
static bool         _online   { false };
static bool         _fileopt  { false };
static std::string_view _file {};
}

std::string_view program_options::get_option(const std::vector<std::string_view>& args,
                                             const std::string_view& option_name){
  for(auto it = args.begin(), end = args.end(); it != end; ++it){
    if(*it == option_name){
      if(it + 1 != end){
        return *(it + 1);
      }
    }
  }

  return "";
}

bool program_options::has_option(const std::vector<std::string_view>& args,
                                 const std::string_view& option_name){
  for(auto it = args.begin(), end = args.end(); it != end; ++it){
    if(*it == option_name){
      return true;
    }
  }

  return false;
}

void program_options::parse(int argc, char* argv[]){
  if(argc > 64){
    throw std::runtime_error("too many input parameters!");
  }

  const std::vector<std::string_view> args(argv, argv + argc);

  for(auto arg = args.begin(), end = args.end(); arg != end; ++arg){
    if(*arg == "-o" || *arg == "--online"){
      if(_online){
        throw std::runtime_error("Cannot use -o/--online param twice!");
      }
      _online = true;
      continue;
    }

    if(*arg == "-f" || *arg == "--file"){
      if(_fileopt){
        throw std::runtime_error("Cannot use -f/--file param twice!");
      }
      if(!_online){
        throw std::runtime_error("Cannot use -f/--file without first specifying -o/--online");
      }

      if(arg + 1 != end){
        _fileopt = true;
        _file = *(arg + 1);
        continue;
      }
      throw std::runtime_error("Option -f/--file needs an argument!");
    }

  }

}

bool program_options::online(){
  return _online;
}

bool program_options::fileopt(){
  return _fileopt;
}

const std::string_view& program_options::file(){
  return _file;
}
