#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace thylacine {

/* Defines interface for our I/O device */
class Server {
public:
  Server(int port);
  ~Server();

  // socket();
  // bind();
  // recvfrom();
  // sendto();

private:
  int port_;
  int sockfd_;
  
  struct addrinfo hints_;  // used to pre-configure socket address structures
  struct addrinfo *res_;   // holds results of library call getaddrinfo()
};

}; // namespace thylacine

#endif // header guard