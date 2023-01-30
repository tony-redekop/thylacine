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

**Server**

1.  Download & install dependencies
2.  `$ cd thylacine`
3.  `$ cd server`
4.  `$ mkdir build`
5.  `$ cd build`
6.  `$ cmake -G "Unix Makefiles" ..`
7.  `$ make`
8.  `$ ./test/runtest`

**Client**

1.  `$ cd thylacine`
2.  `$ cd client`
3.  `$ chmod +x client.py`
4.  `$ ./client.py`

### Usage

See /test/test.cpp for usage examples.

---

Written by Antonio Redekop