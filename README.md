Jake Mitchell Networks Programming Assignment 1

For the ease of working on this during testing, I kept all of the files under one directory.  The make file will clean all executables with clean, make both server and client with make, or can do them individually.

The Client works on a loop system, it takes an input and parses it to check if it is a valid command, if it is, it sends the command to the server.  It then waits for a response.  If it is a get, it first recieves either the number of packets that will be sent, or a file does not exist.  It then gets each packet in turn and saves that to the file.  If it is a put, it opens the file if it exists, determines the number of packets it needs, sends that number, then sends each packet in turn.  For delete and ls, it just sends the command to the server and lets it handle them.

The Server is also on a loop, it waits for a client message, parses it and behaves appropriately.  For ls and delete it called the respective linux commands with popen and returns an appropriate message to the client.  For GET it does what the client would do on a put, and vice versa (the server's PUT is the clients GET).  The only difference being that when the client would have just printed, the server sends a response to the client.
