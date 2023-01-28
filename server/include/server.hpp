#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <queue>
#include <string>
#include <unordered_set>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace thylacine {

/* Globals */
constexpr int MAXBUFFLEN = 256;  // device buffer max length
enum State { IDLE, LISTENING };  // possible device states

/* Define valid functions, parameters, and parameter types available to the client */
inline std::unordered_set<std::string> Commands { "START", "STOP" }; 
inline std::map<std::string, std::map<std::string, std::string>> valid_functions = {
  {"ID",   {{}}}, 
  {"TEST", {{"CMD", "Command"},
            {"DURATION", "int"},
            {"RATE", "int"}}}
};

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
  bool parse_tokens(std::queue<std::string>& tokens,
    std::vector<std::map<std::string, std::pair<std::string, std::string>>>& ast);

  static bool bind_socket(int sockfd, struct addrinfo *rp);
  static void *get_inaddr(struct sockaddr *sa);      // returns IPv4 or IPv6 address 
  static bool validate_msg(const std::string& msg);  // ensure message is well-formed
  static void tokenize_msg(const std::string &msg,      // extract tokens
    std::queue<std::string>& tokens, const char delimiter);
};

}; // namespace thylacine

#endif // header guard