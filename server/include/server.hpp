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

  int create_socket(unsigned timeout);

  static bool bind_socket(int sockfd, struct addrinfo *rp);
  static void *get_inaddr(struct sockaddr *sa);      // returns IPv4 or IPv6 address 
  static bool validate_msg(const std::string& msg);  // ensure message is well-formed
  static void parse_msg(const std::string &msg,      // extract tokens
    std::queue<std::string>& tokens, const char delimiter);
};

}; // namespace thylacine

#endif // header guard