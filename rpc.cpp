#include "rpc.h"

//get sockaddr, IPv4 or IPv6
void* RPC::get_in_addr(struct sockaddr* sa){ 
  if(sa->sa_family == AF_INET){
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

int RPC::sendall(int s, char *buf, int *len){
  int total = 0;        // how many bytes we've sent
  int bytesleft = *len; // how many we have left to send
  int n;

  while(total < *len) {
    n = send(s, buf+total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }

  *len = total; // return number actually sent here

  return (n == -1) ? -1 : 0; // return -1 on failure, 0 on success
} 

void RPC::init(){
  FD_ZERO(&master_fds);
  FD_ZERO(&read_fds);
  FD_SET(STDIN, &master_fds);
  fd_max = STDIN;

  if(program_options::client()){
    c_connect();
    return;
  }

  if(program_options::server()){
    s_init();
    return;
  }
}

void RPC::c_connect(){ //client connect
  std::string text;

  //Try to get address from file
  if(program_options::fileopt()){ 
    std::ifstream file(static_cast<std::string>(program_options::file()), std::ios::in);
    if(!file.is_open()){
      throw std::runtime_error("rpc-cli: Could not open input file: " + static_cast<std::string>(program_options::file()));
      file.close();
    }
    std::getline(file, text);
    file.close();
  } 

  //Try manual input
  if(text.empty()){
    while(true){
      std::cout << "Server address and port [address:port]: ";
      std::cin >> text;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if(std::cin.good()){
        break;
      }
    }
  }

  //Try to parse an address and port from input
  if(text.find(":") == std::string::npos){
    throw std::runtime_error("Could not inperpret server address");
  }

  auto server{ text.substr(0, text.find(":")) };
  auto port{ text.substr(text.find(":")+1, text.back()) };

  //Useful variables for connecting
  struct addrinfo hints, *servinfo, *p;
  int rv;

  memset(&hints, 0, sizeof(hints)); //Zero out hints
  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; //TCP

  if((rv = getaddrinfo(static_cast<std::string>(server).c_str(), static_cast<std::string>(port).c_str(), &hints, &servinfo)) != 0){
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly. Also make sure your opponent is still hosting and waiting for an opponent.");
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
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly. Also make sure your opponent is still hosting and waiting for an opponent.");
    }

    break;
  }

  freeaddrinfo(servinfo); //Done with this data

  if(p == NULL){ // Could not find an addrinfo to connect to
    throw std::runtime_error("Unable to connect to server. Make sure you entered the address and port correctly. Also make sure your opponent is still hosting and waiting for an opponent.");
  }

  FD_SET(m_sockfd, &master_fds);
  if(fd_max<m_sockfd){ fd_max = m_sockfd; }
}

RPC::Move RPC::get_move(){
  if(!program_options::online()){
    std::cout << "What\'s Your Move? (1/2/3)\n";
    std::cout << "1) Rock\n2) Paper\n3) Scissors\n";
  }

  char c;
  while(true){
    std::cin >> c;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(std::cin.good()){
      break;
    }
  }

  //auto p{ std::cout.tellp() };
  std::cout << "\x1b[A" << "\r \r";
  //std::cout << "tellp: " << p;


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

void RPC::get_a(){
  if(m_a != MAX_MOVE){ //Already got a
    return;
  }

  m_a=get_move();

  if( program_options::online() && m_a != MAX_MOVE){
    char buf[2];
    int len{sizeof(buf)};
    reinterpret_cast<uint16_t*>(buf)[0] = htons(m_a);
    sendall(m_sockfd, buf, &len);
  }
}

void RPC::get_b(){
  if(m_b != MAX_MOVE){ //Already got b
    return;
  }

  m_b=get_move();
}

void RPC::print(){
  if(m_a == MAX_MOVE || m_b == MAX_MOVE){
    std::cout << "A Player Has Yet To Make Their Move\n";
    return;
  }

  if(program_options::online()){
    std::cout << "You: " << m_a << '\n';
    std::cout << "Opponent: " << m_b << '\n';
  } else {
    std::cout << "Player A: " << m_a << '\n';
    std::cout << "Player B: " << m_b << '\n';
  }

  if(m_a == m_b){
    std::cout << "TIE!\n";
    return;
  }

  bool aWins;
  switch(m_a){
    case ROCK:
      switch(m_b){
        case PAPER:
          aWins=false;
          break;
        case SCISSORS:
          aWins=true;
          break;
      }
      break;
    case PAPER:
      switch(m_b){
        case ROCK:
          aWins=true;
          break;
        case SCISSORS:
          aWins=false;
          break;
      }
      break;
    case SCISSORS:
      switch(m_b){
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

  if(program_options::online()){
    std::cout << "You " << (aWins ? "Win" : "Lose") << "!\n";
  } else {
    std::cout << "The Winner Is Player: " << (aWins ? "A" : "B") << '\n';
  }
}

//Initialize the Server
void RPC::s_init(){
  std::string text;

  //Try to get port from file
  if(program_options::fileopt()){ 
    std::ifstream file(static_cast<std::string>(program_options::file()), std::ios::in);
    if(!file.is_open()){
      throw std::runtime_error("rpc-cli: Could not open input file: " + static_cast<std::string>(program_options::file()));
      file.close();
    }
    std::getline(file, text);
    file.close();
  }

  //try manual input
  if(text.empty()){
    while(true){
      std::cout << "Open server on port: ";
      std::cin >> text;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if(std::cin.good()){
        break;
      }
    }
  }

  //useful variables for server initialization
  struct addrinfo hints, *servinfo, *p;
  int rv;

  memset(&hints, 0, sizeof(hints)); //Zero out hints
  hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; //TCP
  hints.ai_flags = AI_PASSIVE; //Use our own IP

  if((rv = getaddrinfo(NULL, text.c_str(), &hints, &servinfo)) != 0){
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
    throw std::runtime_error("Unable to open port " + text + ". Make sure nothing else is running on this port");
  }

  //Loop through until we get a connection
  for(p = servinfo; p != NULL; p = p->ai_next){
    if((m_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){ //Try to create socket
      close(m_listener);
      perror("server: socket");
      continue;
    }

    const int yes{1};
    setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // Lose that pesky address already in use message

    if(bind(m_listener, p->ai_addr, p->ai_addrlen) < 0){
      close(m_listener);
      continue;
    }

    break;
  }

  if(p == NULL){ // Could not find an addrinfo to connect to
    throw std::runtime_error("server: failed to bind to port " + text);
  }

  freeaddrinfo(servinfo); //Done with this data

  if(listen(m_listener, 10) == -1){ //Marks as listening
    perror("listen");
    throw std::runtime_error("Could not mark socket as listener");
  }

  FD_SET(m_listener, &master_fds); //Add listener to our master set
  fd_max=m_listener; //Newest socket will be our largest
}

void RPC::s_accept(){
  char remoteIP[INET6_ADDRSTRLEN];
  struct sockaddr_storage remoteaddr;
  socklen_t addrlen = sizeof(remoteaddr);

  if(m_sockfd != -1){
    std::cerr << "A new client tried to connect but we already have an opponent\n";
    close(accept(m_listener, (struct sockaddr*)&remoteaddr, &addrlen));
    return;
  }

  m_sockfd = accept(m_listener, (struct sockaddr*)&remoteaddr, &addrlen);

  if(m_sockfd == -1){
    perror("accept");
    close(m_sockfd);
  } else {
    FD_SET(m_sockfd, &master_fds);
    if(fd_max<m_sockfd){ fd_max = m_sockfd; }
    std::cout << "rpc-cli: server: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << m_sockfd << '\n';
  }
  return;
}

void RPC::handle_fd(int fd){
  //unfortunately cannot use switches on non-constexpr 
  if(fd == m_listener){ //handle connection
    s_accept();
  }

  if(fd == m_sockfd){ //handle data from opponent
    int nbytes;
    char buf[2];
    if((nbytes = recv(fd, buf, sizeof(buf), 0)) <=0){
      //recv
      if(nbytes == 0){
        //connection closed
        std::cout << " rpc-cli: server: socket " << fd << " hung up\n";
      } else {
        perror("recv");
      }
      close(fd);
      FD_CLR(fd, &master_fds);
      std::string err{"rpc-cli: server: socket " + fd};
      err += " hung up";
      throw std::runtime_error(err);
      return;
    } 

    //cast to Move
    m_b = static_cast<Move>(ntohs(*reinterpret_cast<int16_t*>(buf)));

    return;
  }

  if(fd == STDIN){ //handle user input
    if(std::cin.peek() == 'q'){ //Check if user hit q
      throw std::runtime_error("Quitting..."); //I dont want to implement a better solution
    }

    if(m_sockfd == -1){
      std::cout << "Still ";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return;
    }
    get_a();
    return;
  }

  return;
}

void RPC::n_run(){ //Run a networked game of RPC
  while(true){
    if(m_sockfd == -1){
      std::cout << "Waiting for player to join...\n";
    }
    if(m_a == MAX_MOVE && m_sockfd != -1){
      std::cout << "m_a == MAX_MOVE: " << m_a << '\n';
      std::cout << "What\'s Your Move? (1/2/3)\n";
      std::cout << "1) Rock\n2) Paper\n3) Scissors\n";
    } else if(m_sockfd != -1){
      std::cout << "Awaiting opponent...\n";
    }

    read_fds = master_fds;
    if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
      perror("select");
      throw std::runtime_error("Something went wrong when polling sockets");
    }

    for(int i{0}; i<=fd_max; ++i){
      if(FD_ISSET(i, &read_fds)){
        handle_fd(i);
      }
      if(m_a != MAX_MOVE && m_b != MAX_MOVE){
        print();
        return;
      } 
    }
  }
}

//Local run theough events
void RPC::l_run(){
  get_a();
  get_b();
  print();
}

std::ostream& operator<<(std::ostream& out, RPC::Move m){
  switch(m){
    case RPC::Move::ROCK:
      out << "ROCK";
      break;

    case RPC::Move::PAPER:
      out << "PAPER";
      break;

    case RPC::Move::SCISSORS:
      out << "SCISSORS";
      break;

    default:
      out << "ERR";
      break;
  }

  return out;
}
