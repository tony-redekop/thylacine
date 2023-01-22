#include "server.hpp"
#include <iostream>
#include <cassert>

int main() 
{
  { // start TEST 0 
    std::cout << "TEST 0 : INSTANTIATE" << std::endl;

    /* TEST 0 : case 0 : instantiate with valid port number */
    const int port = 4950;
    thylacine::Server sA(port);
    std::cout << "TEST 0 : case 0 : PASS" << std::endl;

    /* TEST 0 : case 1 : instantiate with invalid port number */
    try { 
      const int invalid_port = 65536;
      thylacine::Server sB(invalid_port);  // we expect an exception
      std::cout << "TEST 0 : case 1 : FAIL" << std::endl;
    }
    catch (std::exception &e) {
      std::cout << "TEST 0 : case 1 : PASS" << std::endl;
    }
    
    std::cout << "TEST 0 : COMPLETE" << std::endl;
  } // end TEST 0

  std::cout << std::endl << "ALL TESTS COMPLETE" << std::endl;
  std::cout << "See test.cpp for details" << std::endl;
  std::cout << "Antonio Redekop" << std::endl;

  return 0;
}