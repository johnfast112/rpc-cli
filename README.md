# rpc-cli
A command line Rock Paper Scissors game with online capabilities

# Usage and Options
## Usage
`rpc-cli [OPTIONS]`

## Options
```
-h, --help                  Prints this help text and exits
-s, --server                Asks the user what port to open for an online opponent to use, unless --file option is specified
-c, --connect               Asks the user for an address and port to connect to their online opponent, unless --file option is specified
-f, --file <file>           Reads the provided file for a port number or server address depending on which option was provided. See text files in examples directory for how to format these files
```

## Examples
The `example` directory comes with two files to show how to format an input file with the `--file` option.

These can be used by doing
```
cd examples
rpc-cli --server --file example_port.txt
```
For the server and
```
cd examples
rpc-cli --client --file example_server.txt
```
For the client
