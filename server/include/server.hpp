#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <queue>
#include <string>
#include <cassert>
#include <variant>
#include <iostream>
#include <unordered_set>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define NDEBUG

namespace thylacine {

// Globals 
constexpr int MAXBUFFLEN = 256;  // device buffer max length
enum State { IDLE, LISTENING };  // possible device states

// Defines interface for our I/O device
class Server {
public:
  Server(unsigned port, unsigned timeout = 0);
  ~Server();

  // Server methods (not available to client)
  void listen();

  // Device methods (available to client through UDP)
  template<typename T>
  std::string device_test(std::map<std::string, T>& params); 
  inline std::string device_id();

  // Getters, setters
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

  // Utility functions
  static void *get_inaddr(struct sockaddr *sa);               // returns IPv4 or IPv6 address 
  static bool validate_msg(const std::string& msg);           // ensure message is well-formed
  static void tokenize_msg(const std::string& msg,            // extract tokens
    std::queue<std::string>& tokens, const char delimiter);
  static bool parse_tokens(std::queue<std::string>& tokens,   // validates tokens; builds AST 
    std::map<std::string, std::map<std::string, 
    std::variant<int, std::string>>>& ast);
};

/**
 *  Device test
 */
template<typename T>
std::string Server::device_test(std::map<std::string, T>& params) 
{
  std::string param1 = std::get<std::string>(params["CMD"]);
  int param2 = std::get<int>(params["DURATION"]);
  int param3 = std::get<int>(params["RATE"]);

  return ("device_test() called with parameters: " + param1 + " " +
    std::to_string(param2) + " " + std::to_string(param3));
}

/**
 * Returns device model ID and SN 
 */
std::string Server::device_id() 
{
  
  std::string id{"ID;MODEL=" + id_.first + ";" + "SERIAL=" + id_.second + ";"};
  #ifndef NDEBUG
  std::cout << id << std::endl;
  #endif
  return id;
}

}; // namespace thylacine

#endif // header guard