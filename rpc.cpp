#include "rpc.h"

namespace {
static std::string _server;
static std::string _port;
}

void* RPC::get_in_addr(struct sockaddr* sa){ //get sockaddr, IPv4 or IPv6
  if(sa->sa_family == AF_INET){
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

RPC::Move RPC::getMove(bool p){
  char c;

  while(true){
    std::cout << "What\'s Your Move Player " << (p ? "2" : "1" ) << "? (1/2/3)\n";
    std::cout << "1) Rock\n2) Paper\n3) Scissors\n";

    std::cin >> c;
    std::cout << "\x1b[A\b \b";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(std::cin.good()){ //Try until we have good data
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
      }
    }
  }
}

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
          section += i;
          break;
        case COMMENT:
        default: break;
      }
    }

    if(section.empty()){ //We have garbage before the good stuff
      state = DEFAULT;
      key.clear();
      value.clear();
      continue;
    }

    if(program_options::broadcast()){
      if(section == "broadcast" && !key.empty() && !value.empty()){
        if(key == "port"){
          _port = value;
          return; //Broadcast just needs the port
        }
      }
    }

    if(program_options::connect()){
      if(section == "connect" && !key.empty() && !value.empty()){
        if(key == "server" && _server.empty()){
          _server = value;
        }
        if(key == "port" && _port.empty()){
          _port = value;
        }
      }
    } //No return means we'll just run through the end of the file but it's the user's fault for filling it with 65535 newlines

    state = DEFAULT;
    key.clear();
    value.clear();
  }
}

void RPC::broadcast(){
  FD_ZERO(&master_fds);
  FD_SET(STDIN, &master_fds);
  std::string line;

  //Get our port
  try{
    RPC::parseINI();
  } catch (const std::exception& e){
    std::cerr << e.what() << '\n';
    //try manual input
    if(!program_options::fileopt()){
      while(true){
        std::cout << "Port: ";
        std::cin >> line;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(std::cin.good()){ //Try to get input until it is good
          break;
        }
      }

      _port = line;
    }
  }

  std::cout << "_server: " << _server << '\n';
  std::cout << "_port: " << _port << '\n';

  //Actually setup server
  int rv;
  struct addrinfo hints, *ai, *p;

  memset(&hints, 0, sizeof(hints)); //Zero out hints
  hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE; // Fill with my own IP on NULL ip

  if((rv = getaddrinfo(NULL, _port.c_str(), &hints, &ai)) != 0){
    throw std::runtime_error("server: " + static_cast<std::string>(gai_strerror(rv)));
  }

  for(p = ai; p != NULL; p = p->ai_next) { // Iterate until we can create socket
    if((m_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("server: socket");
      continue;
    }

    const int yes{1}; //Needs const *void
    // lose the pesky "address already in use" error message
    setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if(bind(m_listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(m_listener);
      continue;
    }

    break;
  }

  if(p == NULL){
    throw std::runtime_error("Server: failed to bind");
  }

  freeaddrinfo(ai);

  if(listen(m_listener, 10) == -1){ //Marks as listening
    perror("listen");
    throw std::runtime_error("TODO: Make this a more explicit exception");
  }

  FD_SET(m_listener, &master_fds); // Add listener to our set
  fd_max=m_listener; //Newest socket is largest
}

void RPC::c_connect(){
  std::string line;

  try{
    RPC::parseINI();
  } catch (const std::exception& e){
    std::cerr << e.what() << '\n';
    //try manual input
    if(!program_options::fileopt()){
      while(true){
        std::cout << "Server: ";
        std::cin >> line;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(std::cin.good()){ //Try to get input until it is good
          break;
        }
      }

      _server = line;
      while(true){
        std::cout << "Port: ";
        std::cin >> line;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(std::cin.good()){ //Try to get input until it is good
          break;
        }
      }

      _port = line;
    }
  }

  std::cout << "_server: " << _server << '\n';
  std::cout << "_port: " << _port << '\n';

  int rv;
  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints)); //Zero out hints
  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; //TCP

  if((rv = getaddrinfo(static_cast<std::string>(_server).c_str(), static_cast<std::string>(_port).c_str(), &hints, &servinfo)) != 0){
    
    throw std::runtime_error("getaddrinfo: " + static_cast<std::string>(gai_strerror(rv)) + "Unable to connect to server. Make sure you entered the address and port correctly.");
  }

  //Loop through until we get a connection
  for(p = servinfo; p != NULL; p = p->ai_next){
    if((m_oppfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){ //Try to create socket
      close(m_oppfd);
      perror("client: socket");
      continue;
    }

    if(connect(m_oppfd, p->ai_addr, p->ai_addrlen) == -1){ //Try to connect
      close(m_oppfd);
      perror("client: connect");
      throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
    }

    break;
  }

  freeaddrinfo(servinfo); //Done with this data

  if(p == NULL){ // Could not find an addrinfo to connect to
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly.");
  }
  //TODO: Rest of this
}

void RPC::run(){
  if(!program_options::broadcast() && !program_options::connect()){ //Normie stuff
    m_p1 = RPC::getMove(PLAYER1);
    m_p2 = RPC::getMove(PLAYER2);
    return;
  }

  if(program_options::broadcast()){ //Host
    while(true){
      if(m_p1 == MAX_MOVE){ 
        std::cout << "What\'s Your Move? (1/2/3)\n";
        std::cout << "1) Rock\n2) Paper\n3) Scissors\n";
      } else {
        std::cout << "Waiting for opponent...\n";
      }
      read_fds = master_fds;
      if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){ //Poll our sockets
        perror("select");
        throw std::runtime_error("Something went wrong when polling sockets");
      }

      for(int i{0}; i<=fd_max; ++i){ //Check each socket
        if(FD_ISSET(i, &read_fds)){
          if(i == m_listener){ //New connection
            struct sockaddr_storage remoteaddr; //Local variables that are probably used once. TODO: Double check this if something goes wrong
            socklen_t addrlen = sizeof(remoteaddr);
            char remoteIP[INET6_ADDRSTRLEN];

            m_oppfd = accept(m_listener, (struct sockaddr*)&remoteaddr, &addrlen);

            if(m_oppfd == -1){
              perror("accept");
            } else {
              FD_SET(m_oppfd, &master_fds); //Add our fd
              if(fd_max<m_oppfd){ fd_max = m_oppfd; }
              std::cout << "rpc-cli: server: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << m_oppfd << '\n';
            }
          } else { //Normie data
            if(i == STDIN){
              if(m_p1 != MAX_MOVE){ //Ignore if we already got our input
                std::cout << "\x1b[A\b \b";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              }
              char c;

              std::cin >> c;
              std::cout << "\x1b[A\b \b";
              std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

              if(std::cin.good()){ //Try until we have good data
                switch(c){
                  case '1':
                    m_p1 = ROCK;
                    break;
                  case '2':
                    m_p1 = PAPER;
                    break;
                  case '3':
                    m_p1 = SCISSORS;
                    break;
                }
              }
            }
            if(i == m_oppfd){ //Opponent data
              std::cout << "OPP\n";
              char buf[2];
              int nbytes;
              if((nbytes = recv(m_oppfd, buf, sizeof(buf), 0)) <=0){ //Recieve data
                if(nbytes == 0){
                  //Connection closed
                  std::cout << " rpc-cli: client: socket " << m_oppfd << " hung up\n";
                } else {
                  //Error with recv
                  perror("recv");
                }
                //Close the socket anyway and exit
                close(m_oppfd);
                throw std::runtime_error("Opponent closed connection");
              }

              //TODO: Deal with the data
              std::cout << ntohs(reinterpret_cast<uint16_t*>(buf)[0]) << '\n';
            }
          }
        }
      }
    }
  }
}

void RPC::print(){
  if(m_p1 == MAX_MOVE || m_p2 == MAX_MOVE){
    std::cout << "A Player Has Yet To Make Their Move\n";
    return;
  }

  std::cout << "Player 1: ";
  switch(m_p1){
    case ROCK: std::cout << "ROCK"; break;
    case PAPER: std::cout << "PAPER"; break;
    case SCISSORS: std::cout << "SCISSORS"; break;
  }
  std::cout << '\n';

  std::cout << "Player 2: ";
  switch(m_p2){
    case ROCK: std::cout << "ROCK"; break;
    case PAPER: std::cout << "PAPER"; break;
    case SCISSORS: std::cout << "SCISSORS"; break;
  }
  std::cout << '\n';

  if(m_p1 == m_p2){
    std::cout << "TIE!\n";
    return;
  }

  bool p1;
  switch(m_p1){
    case ROCK:
      switch(m_p2){
        case PAPER:
          p1=false;
          break;
        case SCISSORS:
          p1=true;
          break;
      }
      break;
    case PAPER:
      switch(m_p2){
        case ROCK:
          p1=true;
          break;
        case SCISSORS:
          p1=false;
          break;
      }
      break;
    case SCISSORS:
      switch(m_p2){
        case PAPER:
          p1=true;
          break;
        case ROCK:
          p1=false;
          break;
      }
      break;
    default:
      std::cerr << "WE SHOULDN'T BE HERE AAAH!\n"; //TODO: Should I even deal with this?
      return;
  }

  std::cout << "The Winner Is: " << ( (program_options::connect() || program_options::broadcast()) ? (p1 ? "YOU" : "ENEMY" ) : (p1 ? "Player 1" : "Player 2") ) << '\n';
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
//
//}


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
//    }
//  }
//}
