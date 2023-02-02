#include "server.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  int port = std::stoi(argv[1]);
  int timeout = std::stoi(argv[2]);

  // Bounds checking
  if (port < 1 || port > 65535) {
    throw std::range_error("error: port number out of range");
  }

  thylacine::Server device(port, timeout);
  device.listen();

  return 0;
}