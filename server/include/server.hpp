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

// Globals 
constexpr int MAXBUFFLEN = 256;  // device buffer max length
enum State { IDLE, LISTENING };  // possible device states

// Defines interface for our I/O device
class Server {
public:
  Server(unsigned port, unsigned timeout = 0);
  ~Server();
  void listen();
  inline State get_state() { return state_; }
  inline std::string get_id() { return id_.first; }
  inline std::string get_sn() { return id_.second; }

private:
  unsigned port_;
  unsigned timeout_;      // timeout for recieving (listening) 
  int sockfd_;            // socket descriptor
  State state_;           // device state  
  struct addrinfo *res_;  // holds linked-list of results 
  std::pair<std::string, std::string > id_;  // holds model ID and serial number

  // Static member constants
  static const std::unordered_set<std::string> Commands; 
  static const std::map<std::string, std::map<std::string, std::string>> ParamMap;

  // Socket implementation methods (refactor later to uncouple from state)
  int create_socket(unsigned timeout);
  bool bind_socket(int sockfd, struct addrinfo *rp);

  // Utility functions (may be refactored later into separate classes)
  static void *get_inaddr(struct sockaddr *sa);              // returns IPv4 or IPv6 address 
  static bool validate_msg(const std::string& msg);          // ensure message is well-formed
  static void tokenize_msg(const std::string& msg,           // extract tokens
    std::queue<std::string>& tokens, const char delimiter);
  static bool parse_tokens(std::queue<std::string>& tokens,  // validates tokens; builds AST
    std::vector<std::map<std::string, std::pair<std::string, std::string>>>& ast);
};

}; // namespace thylacine

#endif // header guard