# client-server-model-using-Linux-stream-sockets

It consists of a server that will implement a service (like a chat room) using a simple text protocol that clients can use (through telnet) to message other clients on the service.

Implemented the server as follows:

• The server will set up as a server and establish a socket connection using a port number passed in as an argument to the program. For example, the server and  client code may be executed as follows: ./server <svr_port>
 The server will print out a usage statement and terminate the program if the user does not enter the command with the port as the command-line argument.
 
• The server will support up to and including 10 “registered” client connections (registration is done with the JOIN command). Although more than 10 clients can connect to the server, no more than 10 clients may be registered in the database at the same time. In the case of more than 10 clients attempting to register, the server will print out an error message and simply close the connection. Note that clients may come and go as they please, so if, for example, the 10th client “leaves” the service, space is now made available for another client to register.

• The server has set up a network socket (i.e., AF_INET) over TCP (i.e., SOCK_STREAM) using the port number passed in as an argument to the program.
      The server will bind and listen to the port specified as a command-line argument. When a client connects, a new thread will be spawned to handle the socket         using POSIX threads (i.e., pthreads) in Linux.
      The server will support the following case-sensitive commands which are all sent as plain ASCII text:
      
      § JOIN username
      
      • When a client wants to join (i.e., register for) the service, it first connects to the server using the telnet command with the hostname of the server           machine and the port number, and then sends a JOIN request with the username. Usernames will only consist of alphanumeric characters and will not contain           spaces or other “special” or control characters. You may assume that the user follows this requirement for alphanumeric characters, so no validation is             needed. The server will store this username as well as the client’s socket file descriptor in the “database”. Note that although a client may “connect” to           the server, it MUST register for the service with the JOIN command before the client may receive any “services” provided by the server. If the database is          “full” (i.e., 10 clients have registered for the service), then the server will print out a status message and send a “Too Many Users” message to the client.         once a client has already registered with a JOIN request, any subsequent JOIN requests from the same registered client will be discarded with a status               message sent back to the client.
      
      § LIST
     • If a registered client wants to know who is currently subscribed to the service, the client will issue a LIST request. Upon receipt of the LIST request, the        server will send a list of all registered clients on individual lines and return this list (newlines and all) to the client. Note that the client must be            registered for the service to receive any “services”, such as this one, provided by the server. If the client who is not registered for this service sends a        LIST request, the server will print out a status message and send an “Unregistered User” message to the client with the JOIN request instructions.
     
      § MESG username some_message_text
      • If a registered client wants to send an individual message to another registered client, the client will issue the MESG request with the username of a               registered client followed by whatever message he/she wants to send to the other registered client. The server will then act as a relay and forward this             message to the registered user. Note that the client must be registered for the service to receive any “services”, such as this one, provided by the server.         If the client who is not registered for this service sends a MESG request, the server will print out a status message and send an “Unregistered User”               message to the client with the JOIN request instructions. If a registered client sends a MESG request to an unregistered client, the server will print out a         status message and send an “Unknown Recipient” message to the client.
      
      § BCST some_message_text
    • If a registered client wants to broadcast a message to all other registered clients, the client will issue the BCST request followed by whatever message            he/she wants to send to the other registered clients. The server will then act as a relay and forward this message to the registered users (but not the              sender). Note that the client must be registered for the service to receive any “services”, such as this one, provided by the server. If the client who is not      registered for this service sends a BCST request, the server will print out a status message and send an “Unregistered User” message to the client with the          JOIN request instructions.
    
    
    § QUIT
    • When a connected client wants to leave the service, the client will issue a QUIT request, at which time the server will disconnect the client from the service. The database entry for registered clients should be removed after the client has been disconnected. Note that an unregistered client will still be disconnected from the service (i.e., their connection closed) with a status message at the server, though no data needs to be removed form the database since there is none for that client.
    
    § Unrecognizable Messages
    • If a registered client sends an unrecognizable request (i.e., one not supported by this protocol), the server will print out a status message and send an  
    
    “Unknown Message” message to the client.
    • Note that the client must be registered for the service to receive any “services” provided by the server. If the client who is not registered for this service      sends an unrecognizable message, the server will print out a status message and send an “Unregistered User” message to the client with the JOIN request             instructions.

• The server will provide important status updates of messages sent and received, such as connection events and received requests, identifying the client using their socket file descriptor.

• The server will support error checking across all relevant system calls and other potential issues.
You will simply use a “telnet” client to connect to the server, specifying the hostname and port number of the server to connect to.

• When the telnet client starts up, the client can issue any request (even unrecognizable ones) to the server, but is expected to issue the JOIN request immediately with the client’s username using alphanumeric characters so as to be able to use the services provided by the server.

• When desired, the client will send a LIST request to the server to see who is currently subscribed to the service, but only the registered client will receive the listing (i.e., others will receive an error message with JOIN request instructions).

• When desired, the client will send a MESG or BCST request to the server who will then relay that message to an individual registered client or all registered clients, respectively.

• The client may voluntarily leave the service at any time by issuing the QUIT request.
Program (i.e., a server program) should run on the INET domain using SOCK_STREAM (i.e., TCP) sockets so that the server and client execute on different machines at the same time. The server will accept the port number to communicate on as an argument to the server program. code will handle errors appropriately, such as printing an error message, disconnecting the client, or termination of the program.  
