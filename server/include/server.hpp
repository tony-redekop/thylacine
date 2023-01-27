#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <netdb.h>
#include <queue>
#include <set>

namespace thylacine {

/* Globals */
constexpr int MAXBUFFLEN = 256;         // device buffer max length
const std::set<std::string> Command {  // commands available to the client
  "ID",
  "TEST", 
  "CMD=START", 
  "CMD=STOP"
};
enum State { IDLE, LISTENING };         // possible device states

/* Defines interface for our I/O device */
class Server {
public:
  Server(unsigned port, unsigned timeout = 0);
  ~Server();

  void listen();

  inline State get_state() { return state_; }

private:
  unsigned port_;           
  unsigned timeout_;               // timeout for recieving (listening) 
  int sockfd_;                     // socket descriptor
  State state_;                    // device state  
  struct addrinfo *res_;           // holds linked-list of results 

  /* Message handling */
  void parse_msg(const std::string &msg);  // parse for commands and add to queue
  std::queue<std::string> queue_;          // command queue 

  /* Socket implementation methods */
  int socket(unsigned timeout);
  void bind();
 
  /* Helper functions (not tied to object state) */
  static void *get_inaddr(struct sockaddr *sa);  // returns IPv4 or IPv6 address 
  static int validate_msg(char *msg);            // ensure message is well-formed
};

}; // namespace thylacine

#endif // header guard