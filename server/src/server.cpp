#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.hpp"

namespace thylacine {

Server::Server(int port) : port_{port}, sockfd_{-1}, res_{nullptr},
  state_{State::IDLE}, timeout_{0}
{
  /* Bounds checking */
  if (port < 1 || port > 65535) {
    throw std::range_error("error: port number out of range");
  }

  /* Pre-configure address structures. */
  memset(&hints_, 0, sizeof(hints_)); 
  hints_.ai_family = AF_INET6;        // use IPv6                         
  hints_.ai_socktype = SOCK_DGRAM;    // UDP socket 
  hints_.ai_flags = AI_PASSIVE;       // use host IP address 

  /* Generate address structures */ 
  int rv = getaddrinfo(NULL,          // use host IP address
    std::to_string(port_).c_str(),    // port must be const char *
    &hints_,                          // hints_ used for pre-configuration
    &res_);                           // res_ holds linked-list of addrinfo's
  if (rv != 0) {
    throw std::runtime_error("error: getaddrinfo\n" + std::string{gai_strerror(rv)});
  }
}

Server::~Server() 
{
  if (res_) {
    freeaddrinfo(res_);
    res_ = nullptr;
  }
}

 /* Returns struct in_addr * (IPv4) or struct in6_addr * (IPv6) */
void * Server::get_inaddr(struct sockaddr *p_sa)
{
  if(p_sa->sa_family == AF_INET)
    return &(((struct sockaddr_in *)p_sa)->sin_addr);

  if(p_sa->sa_family == AF_INET6)
    return &(((struct sockaddr_in6 *)p_sa)->sin6_addr);

  return nullptr;
}

/* Validation of message size and last character.  0(1) time complexity.
   Note:  Does not parse or verify if message is well-formed */
int Server::validate_message(char *msg) { 
  size_t len = strlen(msg);           // strlen() does not count null char '\0'
  if(len > 1 && msg[len-1] == ';') {  // shortest possible valid message is "x;"
    return 0;    
  }
  return -1;  // invalid message
}

void Server::bind() 
{
  /* Traverse results until we can successfully create and bind socket.
     Upon loop termination, rp_ points to first valid result or nullptr. */
  for (rp_ = res_; rp_ != nullptr; rp_ = rp_->ai_next) {
    /* Attempt to create socket */
    sockfd_ = socket(rp_->ai_family, 
      rp_->ai_socktype,              
      rp_->ai_protocol);
    if (sockfd_ == -1) {
      std::cerr << "error: create socket" << std::endl;
      continue;
    }

    /* Set socket options */
    struct timeval tv; 
    tv.tv_sec = timeout_;
    tv.tv_usec = 0;

    setsockopt(sockfd_,
      SOL_SOCKET,       // set options at sockets API level
      SO_RCVTIMEO,      // enable recieving timeout
      &tv,
      sizeof(tv)
      );

    /* Attempt to bind socket */
    if (::bind(sockfd_, rp_->ai_addr, rp_->ai_addrlen) == -1) {
      close(sockfd_);
      std::cerr << "error: bind socket" << std::endl;
      continue;
    }

    break;
  } 

  if (rp_ == nullptr) {
    throw std::runtime_error("error: failed to bind socket");
  }
 
  freeaddrinfo(res_);  // we don't need this anymore
  res_ = nullptr;
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
  }
  close(sockfd_);
}

}; // namespace thylacine