#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

//This stuff was stoled straight off of medium.com
//Don't know if this type of behaviour is allowed but here's what the link was as of May 22, 2025:
//https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f

#include <fstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace program_options{
std::string_view get_option(const std::vector<std::string_view>& args,
                            const std::string_view& option_name);
bool has_option(const std::vector<std::string_view>& args,
                const std::string_view& option_name);
void parse(int argc, char* argv[]);

bool server();
bool online();
bool fileopt();
const std::string_view& file();
}; //namespace program_options

#endif
