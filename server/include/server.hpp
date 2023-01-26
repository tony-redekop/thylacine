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
  unsigned timeout_;       // timeout for recieving (listening) 
  int sockfd_;             // socket descriptor
  State state_;            // device state  
  struct addrinfo *res_;   // holds linked-list of results 

  std::queue<std::string> queue_;  // stores commands in FIFO order

  int socket(unsigned timeout);
  void bind();
  void parse_message(const std::string &msg);          // parse message for commands

  /* Helper functions */
  static void *get_inaddr(struct sockaddr *sa); // returns IPv4 or IPv6 address 
  static int validate_message(char *msg);        // ensure message is well-formed
  
};

}; // namespace thylacine

#endif // header guard