#include "server.hpp"
#include <iostream>
#include <cassert>

int main() 
{
  /**
   * START TEST 0 : INSTANTIATION
   */
  { 
    std::cout << "TEST 0 : INSTANTIATE DEVICE" << std::endl;

    // TEST 0 : case 0 : instantiate with valid port number
    const int port = 4950;
    thylacine::Server device_01(port);
    std::cout << "TEST 0 : case 0 : PASS" << std::endl;

    // TEST 0 : case 1 : instantiate with invalid port number
    try { 
      const int invalid_port = 65536;
      thylacine::Server device_02(invalid_port);  // we expect an exception
      std::cout << "TEST 0 : case 1 : FAIL" << std::endl;
    }
    catch (std::exception &e) {
      std::cout << "TEST 0 : case 1 : PASS" << std::endl;
    }
    
    std::cout << "TEST 0 : COMPLETE\n" << std::endl;
  } // end TEST 0

  /**
   * START TEST 1 : RECIEVE UDP PACKETS
   */
  { 
    std::cout << "TEST 1 : LISTEN ON SPECIFIED PORT" << std::endl;

    // TEST 1 : case 0 : listen() on specified port with timeout
    unsigned timeout = 1;  // set timeout in seconds
    thylacine::Server device_01(4955, timeout);
    try {
      device_01.listen();  // we expect an exception due to timeout
      std::cout << "TEST 1 : case 0 : FAIL" << std::endl;
    }
    catch(std::exception &e) {
      std::cout << "TEST 1 : case 0 : PASS" << std::endl;
    }

    /** 
     * Note: Further testing requires use of client or command line utility like netcat.
     * Note: Refactor later to automate testing by fork()ing a child process to call 
     * execv() with netcat or by starting the server using the client
     */
    // TEST 1 : case 1 : listen() on specified port with no timeout 
    thylacine::Server device_02(4960, 0);
    device_02.listen();  // we expect an exception due to timeout
    std::cout << "TEST 1 : case 1 : PASS" << std::endl;
    std::cout << "TEST 1 : COMPLETE" << std::endl;
  } // end TEST 1

  std::cout << std::endl << "ALL TESTS COMPLETE" << std::endl;
  std::cout << "See test.cpp for details" << std::endl;
  std::cout << "Antonio Redekop" << std::endl;

  return 0;
}