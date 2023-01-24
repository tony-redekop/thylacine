#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace thylacine {

enum State { IDLE, LISTENING };

/* Defines interface for our I/O device */
class Server {
public:
  Server(int port);
  ~Server();

  void bind();
  void listen();
  
  inline State get_state() { return state_; }
  inline void set_state(State state) { state_ = state; }
  inline void set_timeout(unsigned s) { timeout_ = s; }

private:
  State state_; 

  int port_;
  int sockfd_;
  unsigned timeout_;
  
  struct addrinfo hints_;  // used to pre-configure socket address structures
  struct addrinfo *res_;   // holds results of library call getaddrinfo()
  struct addrinfo *rp_;    // results pointer points to first valid address struct

  static void * get_inaddr(struct sockaddr *p_sa); // helper function returns IPv4 or IPv6 address 
};

}; // namespace thylacine

#endif // header guard