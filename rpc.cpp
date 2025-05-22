#include "rpc.h"

void RPC::getConnection(){
  std::string text;

  if(program_options::fileopt()){
    std::ifstream file(static_cast<std::string>(program_options::file()), std::ios::in);
    if(!file.is_open()){
      throw std::runtime_error("rpc-cli: Could not open input file: " + static_cast<std::string>(program_options::file()));
    }
    std::getline(file, text);
  } else {
    std::ifstream file(static_cast<std::string>("server.txt"), std::ios::in);
    if(!file.is_open()){
    }
    std::getline(file, text);
  }

  if(text.empty()){
    while(true){
      std::cin >> text;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if(std::cin.good()){
        break;
      }
    }
  }

  std::cout << text.substr(0, text.find(":")) << '\n';
  std::cout << text.substr(text.find(":")+1, text.back()) << '\n';

  if(text.find(":") == std::string::npos){
    throw std::runtime_error("rpc-cli: Could not inperpret server address");
  }

  auto server{ text.substr(0, text.find(":")) };
  auto port{ text.substr(text.find(":")+1, text.back()) };

  struct addrinfo hints, *servinfo, *p;
  int rv;

  memset(&hints, 0, sizeof(hints)); //Zero out hints
  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; //TCP

  if((rv = getaddrinfo(static_cast<std::string>(server).c_str(), static_cast<std::string>(port).c_str(), &hints, &servinfo)) != 0){
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
  }

  //Loop through until we get a connection
  for(p = servinfo; p != NULL; p = p->ai_next){
    if((m_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){ //Try to create socket
      close(m_sockfd);
      perror("client: socket");
      continue;
    }

    if(connect(m_sockfd, p->ai_addr, p->ai_addrlen) == -1){ //Try to connect
      close(m_sockfd);
      perror("client: connect");
      throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
    }

    break;
  }

  freeaddrinfo(servinfo); //Done with this data

  if(p == NULL){ // Could not find an addrinfo to connect to
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
  }

}

RPC::Move RPC::getMove(){
  std::cout << "What\'s Your Move? (1/2/3)\n";
  std::cout << "1) Rock\n2) Paper\n3) Scissors\n";

  char c;
  while(true){
    std::cin >> c;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(std::cin.good()){
      break;
    }
  }

  switch(c){
    case '1':
      return ROCK;
      break;
    case '2':
      return PAPER;
      break;
    case '3':
      return SCISSORS;
      break;
    default:
      return MAX_MOVE;
  }
}

void RPC::getA(){
  m_A=getMove();
}

void RPC::getB(){
  if(program_options::online()){
    m_B=awaitMove();
  } else {
    m_B=getMove();
  }
}

void RPC::print(){
  if(m_A == MAX_MOVE || m_B == MAX_MOVE){
    std::cout << "A Player Has Yet To Make Their Move\n";
    return;
  }

  if(m_A == m_B){
    std::cout << "TIE!\n";
    return;
  }

  bool aWins;
  switch(m_A){
    case ROCK:
      switch(m_B){
        case PAPER:
          aWins=false;
          break;
        case SCISSORS:
          aWins=true;
          break;
      }
      break;
    case PAPER:
      switch(m_B){
        case ROCK:
          aWins=true;
          break;
        case SCISSORS:
          aWins=false;
          break;
      }
      break;
    case SCISSORS:
      switch(m_B){
        case PAPER:
          aWins=true;
          break;
        case ROCK:
          aWins=false;
          break;
      }
      break;
    default:
      std::cerr << "WE SHOULDN'T BE HERE AAAH!\n";
      return;
  }

  std::cout << "The Winner Is: " << (aWins ? "A" : "B") << '\n';
}

RPC::Move RPC::awaitMove(){
  return MAX_MOVE;
}

void RPC::startServer(){
}
