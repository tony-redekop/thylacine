#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.hpp"

namespace thylacine {

/* Define our class invariant (i.e. devices of type Server will always hold 
   a valid UDP socket bound to user-specified port with optional timeout) */
Server::Server(unsigned port, unsigned timeout) : 
  port_{port}, 
  timeout_{timeout}, 
  sockfd_{-1}, 
  res_{nullptr}, 
  state_{State::IDLE}
{
  /* Bounds checking */
  if (port < 1 || port > 65535) {
    throw std::range_error("error: port number out of range");
  }
  /* Pre-configure address structures. */
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); 
  hints.ai_family = AF_INET6;        // use IPv6                         
  hints.ai_socktype = SOCK_DGRAM;    // UDP socket 
  hints.ai_flags = AI_PASSIVE;       // use host IP address 
  /* Generate address structures */ 
  int rv = getaddrinfo(NULL,         // use host IP address
    std::to_string(port_).c_str(),   // port must be const char *
    &hints,                          // hints_ used for pre-configuration
    &res_);                          // res_ holds linked-list of addrinfo's
  if (rv != 0) {
    throw std::runtime_error("error: getaddrinfo\n" + std::string{gai_strerror(rv)});
  }
  /* Make socket with specified timeout, bind to port, and set socket descriptor */
  sockfd_ = create_socket(timeout_);  
}

Server::~Server() 
{
  if (res_) {
    freeaddrinfo(res_);
    res_ = nullptr;
  }
}

 /* Returns struct in_addr * (IPv4) or struct in6_addr * (IPv6) */
void * Server::get_inaddr(struct sockaddr *sa)
{
  if(sa->sa_family == AF_INET)
    return &(((struct sockaddr_in *)sa)->sin_addr);
  if(sa->sa_family == AF_INET6)
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  return nullptr;
}

/* Validates message to ensure it is well-formed (i.e. TOKEN1;TOKEN2=TOKEN3;...) */
bool Server::validate_msg(const std::string& msg)
{ 
  if (msg.empty() || *(msg.end()-1) != ';') { 
    return false;
  }
  std::istringstream iss{msg};                   // convert msg to input stream
  std::string token;            
  while (getline(iss, token, ';')) {             // extract tokens delimited by ';'
    if (token.find('=') != std::string::npos) { 
      std::istringstream sub_iss{token};
      std::string subtoken1, subtoken2;
      getline(sub_iss, subtoken1, '=');     
      getline(sub_iss, subtoken2, '=');  
      if(subtoken2.empty()) {
        return false; 
      }
    }
  }
  return true;
}

/* Recursively tokenize a valid (well-formed) message and add tokens to queue */ 
void Server::tokenize_msg(const std::string& msg, 
  std::queue<std::string>& tokens, const char delimiter)
{
  std::istringstream iss{msg};  // convert msg to input stream
  std::string token;            
  int i = 0;
  while(std::getline(iss, token, delimiter)) {  // delimiter is ';'
    if (i++ > 0) {
      tokenize_msg(token, tokens, '=');
    } else {  // first token 
      std::cout << token << std::endl;
      tokens.push(token);
      continue;
    }
  }
}

/* Traverse link-listed of addrinfo structures and create socket */
int Server::create_socket(unsigned timeout)
{
  int sockfd;
  struct addrinfo *rp = nullptr;
  for (rp = res_; rp != nullptr; rp = rp->ai_next) {
    /* Create socket and return descriptor */
    sockfd = socket(rp->ai_family, 
      rp->ai_socktype,              
      rp->ai_protocol);
    if (sockfd == -1) {
      std::cerr << "error: create socket" << std::endl;
      continue;
    }
    /* Set socket options */
    struct timeval tv; 
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(sockfd,
      SOL_SOCKET,       // set options at sockets API level
      SO_RCVTIMEO,      // enable recieving timeout
      &tv,
      sizeof(tv)
    );
    /* If we reach end of results without success */
    if (rp == nullptr) {
      throw std::runtime_error("error: failed to create / bind socket");
    }
    /* Attempt to bind socket */
    if (!bind_socket(sockfd, rp)) {
      continue;  // keep cycling through linked-list
    }
  }
  /* Upon success */
  return sockfd;
}

bool Server::bind_socket(int sockfd, struct addrinfo *rp) 
{
  if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == -1) { 
    close(sockfd);
    std::cerr << "error: bind socket" << std::endl;
    return false;
  }
  return true;
}

void Server::listen()
{
  char buff[MAXBUFFLEN];
  struct sockaddr_storage client_inaddr;
  socklen_t addr_len = sizeof client_inaddr;  

  std::queue<std::string> tokens{};

  /* Main recieve loop */
  while(1) { 
    /* Note: recvfrom() blocks and returns -1 if no data is recieved before timeout */
    int numbytes = recvfrom(sockfd_,  
      buff,
      MAXBUFFLEN-1,
      0,
      (struct sockaddr *)&client_inaddr, 
      &addr_len);
    if (numbytes == -1) { 
      throw std::runtime_error("error: recvfrom()");
    }
    
    char client_ip[INET6_ADDRSTRLEN];
    inet_ntop(client_inaddr.ss_family,
      get_inaddr((struct sockaddr *)&client_inaddr), client_ip, sizeof client_ip);
    
    buff[numbytes] = '\0';  // important to null terminate the message  

    /* Convert message to std::string */
    std::string msg{buff};
    
    /* Ensure message is well-formed */
    if (!validate_msg(msg)) {
      std::cerr << "server: invalid message recieved from "  << client_ip << std::endl;
      continue;  // ignore invalid message; continue listening
    }
    
    /* Print info */
    std::cout << "server: got a UDP packet from " << client_ip << std::endl; 
    std::cout << "server: packet contains: " << msg << std::endl;
    std::cout << "server: packet is " << numbytes << " bytes long" << std::endl;

    /* Check for "STOP;" command to stop listening and exit loop */
    std::string stop_msg{"STOP;"};
    if (msg == stop_msg) {
      break;
    }

    /* Parse message for tokens and add to tokens queue */
    tokenize_msg(msg, tokens, ';'); 
  }
  close(sockfd_);
}

}; // namespace thylacine