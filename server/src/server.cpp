#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>
#include <csignal>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.hpp"

using std::string;
using std::vector;
using std::map;

namespace thylacine {

/** 
 *  Define static constants 
 */
const std::unordered_set<string> Server::Commands { "START", "STOP" }; 
const std::map<string, std::map<string, string>> Server::ParamMap = {
  {"ID",   {}}, 
  {"TEST", {{"CMD", "Command"},
            {"DURATION", "int"},
            {"RATE", "int"}}}
};

/** 
 * Define class invariant (i.e. devices of type Server will always hold 
 * a valid UDP socket bound to user-specified port with optional timeout)
 */
Server::Server(unsigned port, unsigned timeout) : 
  port_{port}, 
  timeout_{timeout}, 
  sockfd_{-1}, 
  res_{nullptr}, 
  state_{State::IDLE},
  id_{std::make_pair("MOD-A","SN001")}  // fixed ID/SN for now
{
  // Bounds checking
  if (port < 1 || port > 65535) {
    throw std::range_error("error: port number out of range");
  }
  // Pre-configure address structures.
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); 
  hints.ai_family = AF_INET6;        // use IPv6                         
  hints.ai_socktype = SOCK_DGRAM;    // UDP socket 
  hints.ai_flags = AI_PASSIVE;       // use host IP address 
  // Generate address structures  
  int rv = getaddrinfo(NULL,         // use host IP address
    std::to_string(port_).c_str(),   // port must be const char *
    &hints,                          // hints_ used for pre-configuration
    &res_);                          // res_ holds linked-list of addrinfo's
  if (rv != 0) {
    throw std::runtime_error("error: getaddrinfo\n" + std::string{gai_strerror(rv)});
  }
  // Make socket with specified timeout, bind to port, and set socket descriptor
  sockfd_ = create_socket(timeout_);  
}

/**
 * Destroy all humans
 */
Server::~Server() 
{
  // Clean up socket structures
  if (res_) {
    freeaddrinfo(res_);
    res_ = nullptr;
  }
  // Close file descriptors 
  close(sockfd_);
}

/**
 * Returns struct in_addr * (IPv4) or struct in6_addr * (IPv6)
 */
void * Server::get_inaddr(struct sockaddr *sa)
{
  if(sa->sa_family == AF_INET)
    return &(((struct sockaddr_in *)sa)->sin_addr);
  if(sa->sa_family == AF_INET6)
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  return nullptr;
}

/** 
 * Validates message to ensure it is well-formed (i.e. TOKEN1;TOKEN2=TOKEN3;...) 
 */
bool Server::validate_msg(const std::string& msg)
{ 
  // Edge-case: Ignore empty messages or those not terminating with ';'
  if (msg.empty() || *(msg.end()-1) != ';') { 
    return false;
  }
  // Convert message to input stream and check form 
  std::istringstream iss{msg};
  std::string token;            
  while (getline(iss, token, ';')) {  
    if (token.find('=') != std::string::npos) { 
      std::istringstream sub_iss{token};
      std::string subtoken1, subtoken2;
      getline(sub_iss, subtoken1, '=');     
      getline(sub_iss, subtoken2, '=');  
      if(subtoken2.empty()) {
        return false; 
      }
    }
  }
  return true;
}

/** 
 * Recursively tokenize a valid (well-formed) message and add each token to 
 * the queue passed in by reference, creating a stream of tokens.
 */
void Server::tokenize_msg(const std::string& msg, 
  std::queue<std::string>& tokens, const char delimiter)
{
  std::istringstream iss{msg};                 
  std::string token;            
  int i = 0;
  while(std::getline(iss, token, delimiter)) {  
    if (i++ > 0) {
      tokenize_msg(token, tokens, '=');
    } else {  // if first token; exit condition
      #ifndef NDEBUG
      std::cout << token << std::endl;
      #endif
      tokens.push(token);
      continue;
    }
  }
}

/** 
 * Validates a token stream and builds an AST (abstract syntax tree) 
 */
bool Server::parse_tokens(std::queue<string>& tokens,
  std::map<string, std::map<string, std::variant<int, string>>>& ast)
{
  // Check first token for valid function name
  string funcname{tokens.front()};
  if (ParamMap.find(funcname) == ParamMap.end()) {
    std::cerr << "parse_tokens() : Invalid function name" << std::endl; 
    return false;
  } else {        // if valid token
    tokens.pop(); // we are done with first token
  } 

  // Calculate number of parameters required for the given function.
  // unsigned numreq = ParamMap.at(funcname).size()
  unsigned numreq = ParamMap.at(funcname).empty() ? 0 : ParamMap.at(funcname).size();
  #ifndef NDEBUG
  std::cout << "This function requires (" << numreq << ") parameter(s)" << std::endl;
  #endif

  string paramname{}; // holds parameter name
  string paramval{};  // holds parameter value

  // Process remaining tokens and build parameter map
  std::map<std::string, std::variant<int, std::string>> param_map;

  for (int i = 0; i < numreq; ++i) {
    // Validate number of parameters
    if (tokens.empty()) {
      std::cerr << "parse_tokens() : Invalid number of parameters" << std::endl; 
      return false; 
    }

    // Get next token (parameter name) 
    paramname = tokens.front(); 
    // Validate parameter name
    if (ParamMap.at(funcname).find(paramname) == ParamMap.at(funcname).end()) {
      std::cerr << "parse_tokens() : Invalid parameter name" << std::endl; 
      return false;
    } else { // parameter name is valid
      tokens.pop();
    }
    if (tokens.empty()) {
      std::cerr << "parse_tokens() : Missing parameter value" << std::endl; 
      return false; 
    }

    paramval = tokens.front();  // get parameter value

    std::variant<int, string> paramvar;                     // holds parameter value 
    string paramtype{ParamMap.at(funcname).at(paramname)};  // holds the required type 

    // Validate parameter value and parameter type
    if (paramtype == "int") {
      try {
        paramvar = std::stoi(paramval);
      }
      catch(std::invalid_argument& ex) {
        std::cerr << "parse_tokens(): error: invalid arg type, expected 'int'" << std::endl;
        return false;
      }
      catch(std::out_of_range& ex) {
        std::cerr << "parse_tokens(): error: argument value out of range" << std::endl;
        return false;
      }
    } else if (paramtype == "Command") {
        if (Commands.find(paramval) == Commands.end()) {
          std::cerr << "parse_tokens(): error: invalid arg type, expected Command" << std::endl;
          return false;
        }
      paramvar = paramval;
    }
    tokens.pop();  // we are done with this token

    // Capture parameter name and value in our map 
    param_map[paramname] = paramvar;

    // Continue iterating through token stream 
  }

  // Build our AST (abstract syntax tree)
  ast[funcname] = param_map;

  // Success!  We can now use our AST to build function calls
  return true; 
}

/** 
 * Traverse link-listed of addrinfo structures and create socket
 */
int Server::create_socket(unsigned timeout)
{
  int sockfd;
  struct addrinfo *rp = nullptr;
  for (rp = res_; rp != nullptr; rp = rp->ai_next) {
    // Create socket and return descriptor
    sockfd = socket(rp->ai_family, 
      rp->ai_socktype,              
      rp->ai_protocol);
    if (sockfd == -1) {
      std::cerr << "error: create socket" << std::endl;
      continue;
    }
    // Set socket options
    struct timeval tv; 
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(sockfd,
      SOL_SOCKET,       // set options at sockets API level
      SO_RCVTIMEO,      // enable recieving timeout
      &tv,
      sizeof(tv)
    );
    int yes = 1;
    setsockopt(sockfd,  // allows re-use of local addresses and ports
      SOL_SOCKET,
      SO_REUSEADDR,
      &yes,
      sizeof(yes)
    );

    // If we reach end of results without success
    if (rp == nullptr) {
      throw std::runtime_error("error: failed to create / bind socket");
    }
    // Attempt to bind socket
    if (!bind_socket(sockfd, rp)) {
      continue;  // keep cycling through linked-list
    }
  }
  // Upon success
  return sockfd;
}

/**
 * Binds socket
 */
bool Server::bind_socket(int sockfd, struct addrinfo *rp) 
{
  if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == -1) { 
    close(sockfd);
    std::cerr << "error: bind socket" << std::endl;
    return false;
  }
  return true;
}

/**
 *  Start server to listen for UDP packets on specified port
 */
void Server::listen()
{
  char buff[MAXBUFFLEN];
  struct sockaddr_storage client_inaddr;
  socklen_t addr_len = sizeof client_inaddr;  

  std::queue<string> tokens{};  // holds "stream" of tokens

  // Initialize pid to 0
  pid_t pid = 0;

  // Main recieve loop
  while (1) { 
    // Note: recvfrom() blocks and returns -1 if no data is recieved before timeout
    int numbytes = recvfrom(sockfd_,  
      buff,
      MAXBUFFLEN-1,
      0,
      (struct sockaddr *)&client_inaddr, 
      &addr_len);
    if (numbytes == -1) { 
      throw std::runtime_error("error: recvfrom()");
    }
    
    char client_ip[INET6_ADDRSTRLEN];
    inet_ntop(client_inaddr.ss_family,
      get_inaddr((struct sockaddr *)&client_inaddr), client_ip, sizeof client_ip);
    
    buff[numbytes] = '\0';  // important to null terminate the message  

    // Convert message to std::string
    std::string msg{buff};
    
    // Ensure message is well-formed
    if (!validate_msg(msg)) {
      std::cerr << "server: invalid message recieved from "  << client_ip << std::endl;
      continue;  // ignore invalid message; continue listening
    }
    
    // Print info 
    #ifndef NDEBUG
    std::cout << "server: got a UDP packet from " << client_ip << std::endl; 
    std::cout << "server: packet contains: " << msg << std::endl;
    std::cout << "server: packet is " << numbytes << " bytes long" << std::endl;
    #endif

    // Check for "STOP;" command to stop listening and exit loop
    std::string stop_msg{"STOP;"};
    if (msg == stop_msg) {
      sendto(sockfd_, "STATUS;STATE=IDLE;", strlen("STATUS;STATE=IDLE;"),
        0, (struct sockaddr *)&client_inaddr, addr_len);
      break;
    }

    // Parse message for tokens and add to tokens queue
    tokenize_msg(msg, tokens, ';'); 
    
    // AST (abstract syntax tree) 
    // Note: refactor location of definition later 
    std::map<string, std::map<string, std::variant<int, std::string>>> ast;     

    // Parse tokens to populate AST 
    if (!parse_tokens(tokens, ast)) {
      std::cerr << "server: error invalid tokens" << std::endl;
      tokens = {};  // clear our queue of tokens
      continue;     // continue listening
    } else {
      #ifndef NDEBUG
      std::cout << "server: all tokens parsed!" << std::endl;
      #endif
    }

    int wstatus;
    if (pid > 0) {
      if (ast.find("TEST") != ast.end() &&
        std::get<string>(ast.at("TEST").at("CMD")) == "STOP") {
          kill(pid, SIGTERM);
          sendto(sockfd_, "TEST;RESULT=STOPPED;", strlen("TEST;RESULT=STOPPED;"),
            0, (struct sockaddr *)&client_inaddr, addr_len);
          continue;
      } else {  // if we don't have STOP command
        waitpid(pid, &wstatus, 0);
      }
    }

    pid = fork();

    if (pid > 0) {
      continue;
    }
    if (pid == 0) {  // child process
      // Use AST to generate function call
      std::string result;
      if (ast.find("TEST") != ast.end() &&
        std::get<string>(ast.at("TEST").at("CMD")) == "START") {
          const char *msg = "TEST;RESULT=STARTED;";
          sendto(sockfd_, msg, strlen(msg),
            0, (struct sockaddr *)&client_inaddr, addr_len);
          result = device_test(ast.at("TEST"));  // call our function
      } else if (ast.find("ID") != ast.end()) {
          result = device_id();
      }
      
      sendto(sockfd_, result.c_str(), strlen(result.c_str()),
          0, (struct sockaddr *)&client_inaddr, addr_len);

      exit(0);
    }
  }
}

}; // namespace thylacine