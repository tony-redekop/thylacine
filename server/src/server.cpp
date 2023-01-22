#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include "server.hpp"

namespace thylacine {

Server::Server(int port) : port_{port}, sockfd_{-1}, res_{nullptr}
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

}; // namespace thylacine