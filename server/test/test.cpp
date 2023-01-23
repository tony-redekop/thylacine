#include "server.hpp"
#include <iostream>
#include <cassert>

int main() 
{
  { // start TEST 0 
    std::cout << "TEST 0 : INSTANTIATE DEVICE" << std::endl;

    /* TEST 0 : case 0 : instantiate with valid port number */
    const int port = 4950;
    thylacine::Server device_01(port);
    std::cout << "TEST 0 : case 0 : PASS" << std::endl;

    /* TEST 0 : case 1 : instantiate with invalid port number */
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

  { // start TEST 1
    std::cout << "TEST 1 : CREATE AND BIND SOCKET" << std::endl;
    
    /* TEST 1 : case 0 : create and bind socket to specified port */
    const int port = 4950;
    thylacine::Server device_01(port);  
    assert(device_01.get_state() == thylacine::State::INACTIVE);
    
    device_01.bind();
    assert(device_01.get_state() == thylacine::State::BOUND);

    std::cout << "TEST 1 : case 0 : PASS" << std::endl;
    std::cout << "TEST 1 : COMPLETE\n" << std::endl;
  } // end TEST 1

  { // start TEST 2
    std::cout << "TEST 2 : LISTEN ON SPECIFIED PORT" << std::endl;

    /* TEST 2 : case 0 : listen() on specified port for s seconds */
    thylacine::Server device_01(4955);
    unsigned s = 1;  
    device_01.set_timeout(s);  // configure device to "listen" for s seconds
    assert(device_01.get_state() == thylacine::State::INACTIVE);

    device_01.bind();
    assert(device_01.get_state() == thylacine::State::BOUND);

    try {
      device_01.listen(); // we expect an exception due to timeout
      // assert(device_01.get_state() == thylacine::State::LISTENING);
      std::cout << "TEST 2 : case 0 : FAIL" << std::endl;
    }
    catch(std::exception &e) {
      std::cout << "TEST 2 : case 0 : PASS" << std::endl;
    }
    std::cout << "TEST 2 : COMPLETE" << std::endl;
  } // end TEST 2

  std::cout << std::endl << "ALL TESTS COMPLETE" << std::endl;
  std::cout << "See test.cpp for details" << std::endl;
  std::cout << "Antonio Redekop" << std::endl;

  return 0;
}