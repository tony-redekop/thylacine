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
- **PySide6** minimum 6.4.2

### Instructions

**Build Instructions**

1.  Download & install dependencies
2.  `$ cd thylacine`
3.  `$ cd server`
4.  `$ mkdir build`
5.  `$ cd build`
6.  `$ cmake -G "Unix Makefiles" ..`
7.  `$ make`

**Client**

1.  Build the server per Build Instructions 
2.  `$ cd thylacine`
3.  `$ cd client`
4.  `$ chmod +x client.py`
5.  `python -m venv env`
6.  `source env/bin/activate`
7.  `pip install -r requirements.txt`
8.  `$ ./client.py`
9.  `deactivate`

**Server**

Note: The client starts the server process. No need to run
the server directly.

1.  `$ ./src/demo`

**Unit Tests**
1.  `$ ./test/runtest`

### Usage

Coming soon...

---

Written by Antonio Redekop