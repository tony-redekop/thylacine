#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace thylacine {

enum State { INACTIVE, BOUND };

/* Defines interface for our I/O device */
class Server {
public:
  Server(int port);
  ~Server();

  void bind();
  
  inline State get_state() { return state_; }
  inline void set_state(State state) { state_ = state; }

private:
  State state_; 

  int port_;
  int sockfd_;
  
  struct addrinfo hints_;  // used to pre-configure socket address structures
  struct addrinfo *res_;   // holds results of library call getaddrinfo()
  struct addrinfo *rp_;    // results pointer points to first valid address struct
};

}; // namespace thylacine

#endif // header guard