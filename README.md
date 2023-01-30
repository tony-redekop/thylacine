# thylacine

Simple UDP server and client demo to simulate communication with I/O devices

### Goals

1. Write a program (client) that allows the user to :
  - connect with a test device
  - define test duration
  - start a test
  - force a test to stop early
  - see a live plot of measured values during test

2. Write a program (server) that simulates the test device
  - the device must "listen" for UDP packets, ignoring unknown messages
  - must be able to run multiple concurrent devices on same host   
  - unique port number should be specified for each each instance

### Dependencies
- **cmake** minimum 3.14

### Instructions

1.  Download & install dependencies
2.  `$ cd thylacine`
3.  `$ mkdir build`
4.  `$ cd build`
5.  `$ cmake -G "Unix Makefiles" ..`
6.  `$ make`
7.  `$ ./test/runtest`

### Usage

See /test/test.cpp for usage examples.

---

Written by Antonio Redekop