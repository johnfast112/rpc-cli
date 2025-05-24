#include "rpc.h"

namespace {
static std::string_view _server;
static std::string_view _port;
}

void* RPC::get_in_addr(struct sockaddr* sa){ //get sockaddr, IPv4 or IPv6
  if(sa->sa_family == AF_INET){
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

//int RPC::sendall(int s, char *buf, int *len){
//  int total = 0;        // how many bytes we've sent
//  int bytesleft = *len; // how many we have left to send
//  int n;
//
//  while(total < *len) {
//    n = send(s, buf+total, bytesleft, 0);
//    if (n == -1) { break; }
//    total += n;
//    bytesleft -= n;
//  }
//
//  *len = total; // return number actually sent here
//
//  return n==-1?-1:0; // return -1 on failure, 0 on success
//} 

void RPC::parseINI(){
  std::string line;
  std::ifstream file;
  State state{DEFAULT};

  std::string section;
  std::string key;
  std::string value;

  //Open the ini file
  if(!program_options::file().empty()){ //Try with specific file
    file.open(static_cast<std::string>(program_options::file()), std::ios::in); //Need to cast string_view to string unfortunately
    if(!file.is_open()){
      throw std::runtime_error("rpc-cli: Could not open input file: " + static_cast<std::string>(program_options::file()));
    }
  } else { //try default file
    file.open("rpc.ini", std::ios::in);
    if(!file.is_open()){
      throw std::runtime_error("Could not open default file rpc.ini");
    }
  }

  //Parse the file
  while(std::getline(file, line)){ //Line by Line
    for(auto i : line){
      switch(state){
        case DEFAULT:
          if(i == '['){ state = SECTION; section.clear(); break; }
          if(i == ';'){ state = COMMENT; break; }
          if(i == ' '){ break; } //Just ignore spaces
          key += i; state = KEY; break;
        case KEY:
          if(i == '='){ state = VALUE; break; }
          if(i == ' '){ break; }
          key += i; break;
        case VALUE:
          value += i; break;
        case SECTION:
          if(i == ']'){ state = COMMENT; break; } //I really don't feel like making sure you've got a REALLY good ini file
          break;
      }
    }

    if(section.empty()){ //We have garbage before the good stuff
      state = DEFAULT;
      key.clear();
      value.clear();
      continue;
    }

    //TODO: Actually parse the keys and sections

    state = DEFAULT;
    key.clear();
    value.clear();
  }
}

void RPC::broadcast(){
  std::string line;

  try{
    RPC::parseINI();
  } catch (const std::exception& e){
    std::cout << e.what() << '\n';
    //try manual input
    if(!program_options::fileopt()){
      while(true){
        std::cout << "Port: ";
        std::cin >> line;
        std::cout << "line: " << line << '\n'; //TODO: Remove this
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(std::cin.good()){ //Try to get input until it is good
          break;
        }
      }

      _port = line;
    }
  }

}

//  struct addrinfo hints, *servinfo, *p;
//  int rv;
//
//  memset(&hints, 0, sizeof(hints)); //Zero out hints
//  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
//  hints.ai_socktype = SOCK_STREAM; //TCP
//  hints.ai_flags = AI_PASSIVE; //Use our own IP
//
//  if((rv = getaddrinfo(NULL, text.c_str(), &hints, &servinfo)) != 0){
//    std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
//    throw std::runtime_error("Unable to create server. Make sure nothing else is running on this port.");
//  }
//
//  //Loop through until we get a connection
//  for(p = servinfo; p != NULL; p = p->ai_next){
//    if((m_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){ //Try to create socket
//      close(m_listener);
//      perror("server: socket");
//      continue;
//    }
//
//    const int yes{1};
//    setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // Lose that pesky address already in use message
//
//    if(bind(m_listener, p->ai_addr, p->ai_addrlen) < 0){
//      close(m_listener);
//      continue;
//    }
//
//    break;
//  }
//
//  if(p == NULL){ // Could not find an addrinfo to connect to
//    throw std::runtime_error("server: failed to bind");
//  }
//
//  freeaddrinfo(servinfo); //Done with this data
//
//  if(listen(m_listener, 10) == -1){ //Marks as listening
//    perror("listen");
//    throw std::runtime_error("Could not mark socket as listener");
//  }
//
//  FD_SET(m_listener, &master_fds); //Add listener to our master set
//  fd_max=m_listener; //Newest socket will be our largest

void RPC::connect(){
}

//void RPC::connect(){ //client connect
//  std::string text;
//
//  if(program_options::fileopt()){ //Check if file specified
//    std::ifstream file(static_cast<std::string>(program_options::file()), std::ios::in);
//    if(!file.is_open()){
//      throw std::runtime_error("rpc-cli: Could not open input file: " + static_cast<std::string>(program_options::file()));
//    }
//    std::getline(file, text);
//  } else { //Try the default
//    std::ifstream file(static_cast<std::string>("server.txt"), std::ios::in);
//    if(!file.is_open()){
//    }
//    std::getline(file, text);
//  }
//
//  //Try manual input
//  if(text.empty()){
//    while(true){
//      std::cin >> text;
//      std::cin.clear();
//      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//
//      if(std::cin.good()){
//        break;
//      }
//    }
//  }
//
//  if(text.find(":") == std::string::npos){
//    throw std::runtime_error("rpc-cli: Could not inperpret server address");
//  }
//
//  auto server{ text.substr(0, text.find(":")) };
//  auto port{ text.substr(text.find(":")+1, text.back()) };
//
//  struct addrinfo hints, *servinfo, *p;
//  int rv;
//
//  memset(&hints, 0, sizeof(hints)); //Zero out hints
//  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
//  hints.ai_socktype = SOCK_STREAM; //TCP
//
//  if((rv = getaddrinfo(static_cast<std::string>(server).c_str(), static_cast<std::string>(port).c_str(), &hints, &servinfo)) != 0){
//    std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
//    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
//  }
//
//  //Loop through until we get a connection
//  for(p = servinfo; p != NULL; p = p->ai_next){
//    if((m_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){ //Try to create socket
//      close(m_sockfd);
//      perror("client: socket");
//      continue;
//    }
//
//    if(connect(m_sockfd, p->ai_addr, p->ai_addrlen) == -1){ //Try to connect
//      close(m_sockfd);
//      perror("client: connect");
//      throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
//    }
//
//    break;
//  }
//
//  freeaddrinfo(servinfo); //Done with this data
//
//  if(p == NULL){ // Could not find an addrinfo to connect to
//    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
//  }
//
//}

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

//void RPC::getA(){
//  std::cout << "m_sockfd" << sizeof(m_sockfd) << '\n';
//  std::cout << "m_listener" << sizeof(m_listener) << '\n';
//
//  std::cout << "master_fds" << sizeof(master_fds) << '\n';
//  std::cout << "read_fds" << sizeof(read_fds) << '\n';
//
//  std::cout << "fd_max" << sizeof(fd_max) << '\n';
//
//  std::cout << "m_A" << sizeof(m_A) << '\n';
//  std::cout << "m_B" << sizeof(m_B) << '\n';
//  m_A=getMove();
//  if(program_options::online()){
//    char buf[2];
//    int len{sizeof(buf)};
//    reinterpret_cast<uint16_t*>(buf)[0] = htons(m_A);
//    sendall(m_sockfd, buf, &len);
//  }
//}
//
//void RPC::getB(){
//  if(program_options::online()){
//    m_B=static_cast<RPC::Move>(ntohs(awaitMove()));
//  } else {
//    m_B=getMove();
//  }
//}

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
      std::cerr << "WE SHOULDN'T BE HERE AAAH!\n"; //TODO: Should I even deal with this?
      return;
  }

  std::cout << "The Winner Is: " << (aWins ? "A" : "B") << '\n';
}

//uint16_t RPC::awaitMove(){ //TODO: Make this stub func
//  int nbytes;
//  char buf[2];
//  fd_set server;
//  FD_ZERO(&server);
//  FD_SET(m_sockfd, &server);
//  if(select(m_sockfd+1, &server, NULL, NULL, NULL) == -1){
//    perror("select");
//    throw std::runtime_error("Something went wrong when polling sockets");
//  }
//
//  if(FD_ISSET(m_sockfd, &server)){
//    if((nbytes = recv(m_sockfd, buf, sizeof(buf), 0)) <=0){
//      if(nbytes == 0){
//        //connection closed
//        std::cout << " rpc-cli: client: socket " << m_sockfd << " hung up\n";
//      } else {
//        perror("recv");
//      }
//      close(m_sockfd);
//      throw std::runtime_error("Server closed connection");
//    }
//  }
//
//  return reinterpret_cast<uint16_t*>(buf)[0];
//}

//void RPC::startServer(){ //Run this before s_listen()
//}

//void RPC::s_listen(){
//  int newfd;
//  struct sockaddr_storage remoteaddr;
//  socklen_t addrlen;
//
//  char buf[4];
//  int nbytes;
//
//  uint16_t mA{0};
//  uint16_t mB{0};
//
//  char remoteIP[INET6_ADDRSTRLEN];
//
//  while(true){
//    read_fds = master_fds;
//    if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
//      perror("select");
//      throw std::runtime_error("Something went wrong when polling sockets");
//    }
//
//    for(int i{0}; i<=fd_max; ++i){
//      if(FD_ISSET(i, &read_fds)){
//        if(i == m_listener){
//          addrlen = sizeof(remoteaddr);
//          newfd = accept(m_listener, (struct sockaddr*)&remoteaddr, &addrlen);
//
//          if(newfd == -1){
//            perror("accept");
//          } else {
//            FD_SET(newfd, &master_fds);
//            if(fd_max<newfd){ fd_max = newfd; }
//            std::cout << "rpc-cli: server: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << newfd << '\n';
//          }
//        } else {
//          //handle data from our client
//          if((nbytes = recv(i, buf, sizeof(buf), 0)) <=0){
//            if(nbytes == 0){
//              //connection closed
//              std::cout << " rpc-cli: server: socket " << i << " hung up\n";
//            } else {
//              perror("recv");
//            }
//            close(i);
//            FD_CLR(i, &master_fds);
//          } else {
//            if(mA == 0){
//              mA = reinterpret_cast<uint16_t*>(buf)[0];
//            } else if(mB == 0){
//              mB = reinterpret_cast<uint16_t*>(buf)[0];
//              for(int j{0}; j<fd_max; ++j){
//                //Send to everyone
//                if (FD_ISSET(j, &master_fds)) {
//                  // except the listener and ourselves
//                  if (j != m_listener && j != i) {
//                    reinterpret_cast<uint16_t*>(buf)[0] = mB;
//                    if (sendall(j, buf, &nbytes) == -1) {
//                      perror("send");
//                    }
//                  } else if(j != m_listener){
//                    reinterpret_cast<uint16_t*>(buf)[0] = mA;
//                    if (sendall(j, buf, &nbytes) == -1) {
//                      perror("send");
//                    }
//                  }
//                }
//              }
//            }
//          }
//        }
//      }
//    }
//  }
//}
