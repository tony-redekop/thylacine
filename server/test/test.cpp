#include "server.hpp"
#include <iostream>
#include <cassert>

int main() {
  std::cout << "TEST 0 : INITIALIZE" << std::endl;
  // TEST 0 : case 0 : do nothing yet 
  {
    assert(true);
    std::cout << "TEST 0 : case 0 : PASS" << std::endl;
  }
  std::cout << "TEST 0 : COMPLETE" << std::endl;
  std::cout << std::endl << "ALL TESTS COMPLETE" << std::endl;
  std::cout << "See test.cpp for details" << std::endl;
  std::cout << "Antonio Redekop" << std::endl;
  return 0;
}