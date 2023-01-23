#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include "server.hpp"

namespace thylacine {

Server::Server(int port) : port_{port}, sockfd_{-1}, res_{nullptr},
  state_{State::INACTIVE}, timeout_{0}
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
 
  set_state(State::BOUND);

  freeaddrinfo(res_);  // we don't need this anymore
  res_ = nullptr;
}

void Server::listen()
{
  const int MAXBUFFLEN = 1000;
  char buff[MAXBUFFLEN];
  struct sockaddr_storage client_addr;
  socklen_t addr_len = sizeof client_addr;  
  
  /* Note: recvfrom() is a blocking function */
  int numbytes = recvfrom(sockfd_,  
    buff,
    MAXBUFFLEN-1,
    0,
    (struct sockaddr *)&client_addr, 
    &addr_len);
  if (numbytes == -1) {
    throw std::runtime_error("error: recvfrom()");
  }
}

}; // namespace thylacine