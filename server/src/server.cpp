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

  sockfd_ = socket(timeout_);  // make socket and return socket descriptor
  bind();                      // binds socket to user specified port

  /* Our class invariant is fully defined now (i.e.: devices of type 
     Server will always hold a valid UDP socket bound to user-specified 
     port with optional timeout. Default device state is State::IDLE */
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

/* Validation of message size and last character.  0(1) time complexity.
   Note:  Does not parse; only verifies if message is well-formed */
int Server::validate_message(char *msg)
{ 
  size_t len = strlen(msg);           // strlen() does not count null char '\0'
  if(len > 1 && msg[len-1] == ';') {  // shortest possible valid message is "x;"
    return 0;    
  }
  return -1;  // invalid message
}

/* Parses a validated message for commands and adds to command queue */
void Server::parse_message(const std::string& msg)
{
  std::istringstream iss{msg};
  std::string token;
  int i = 0;
  while(std::getline(iss, token, ';')) {  // delimiter is ';'
    // std::cout << "parse_message(): " << token << std::endl;
    queue_.push(token);
  }
}

int Server::socket(unsigned timeout)
{
  /* Traverse link-listed of addrinfo structures and create socket */
  int sockfd;
  struct addrinfo *rp = nullptr;
  for (rp = res_; rp != nullptr; rp = rp->ai_next) {
    /* Create socket and return descriptor */
    sockfd = ::socket(rp->ai_family, 
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
  }
  return sockfd;
}

void Server::bind() 
{
  /* Traverse link-listed of addrinfo structures and bind to first valid socket */
  struct addrinfo *rp = nullptr;
  for (rp = res_; rp != nullptr; rp = rp->ai_next) {
    if (::bind(sockfd_, rp->ai_addr, rp->ai_addrlen) == -1) { 
      close(sockfd_);
      std::cerr << "error: bind socket" << std::endl;
      continue;
    } else {          // if success
      break; 
    }
  } 
  if (rp == nullptr)  // if unable to bind with any result
    throw std::runtime_error("error: failed to bind socket");
}

void Server::listen()
{
  char buff[MAXBUFFLEN];
  struct sockaddr_storage client_inaddr;
  socklen_t addr_len = sizeof client_inaddr;  

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
    
    /* Ensure message is well-formed */
    if (validate_message(buff) != 0) {
      std::cerr << "server: invalid message recieved from "  << client_ip << std::endl;
      continue;  // ignore invalid message and continue listening
    }
    
    /* Print info */
    std::cout << "server: got a UDP packet from " << client_ip << std::endl; 
    std::cout << "server: packet contains: " << buff << std::endl;
    std::cout << "server: packet is " << numbytes << " bytes long" << std::endl;

    /* Check for "STOP;" command to stop listening and exit loop */
    std::string stop_msg{"STOP;"};
    if (stop_msg == buff) {
      break;
    }

    /* Parse message for commands */
    std::string msg{buff};
    parse_message(msg);

  }
  close(sockfd_);
}

}; // namespace thylacine