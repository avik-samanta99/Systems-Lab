/*
===============
Clent side code
===============
*/

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<vector>
#include<map>
using namespace std;

int main(int argc, char *argv[]){
    
    // We need three arguments at the command-line
    // 1. Filename, 2. Server's IP address, 3. Server's Port number
    if(argc < 3){
        cout << "Error! Not enough information provided at the command line.\n";
        exit(0);
    }
    
    // Some variable which we will need later in our programme
    int socketfd; // File descriptor for the socket (0 : if created, -1 : if failed)
    struct sockaddr_in ServerAddress; // Server's address (Proxy server)
    int addressLength = sizeof(ServerAddress); // Lengtho or size of the address
    char bufferRequest[1024], bufferResposne[1024]; // buffer for the request and response message
    
    // Port number from the command line
    int portNo = stoi(argv[2]);
    // server's ip address from the comand line
    char *serverIP = argv[1];
    
    // Creating the socket with AF_INET (IPv4), SOCK_STREAM (TCP), Default Internet Protocol (0)
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // file descriptor, 0 : success, -1 : failure
    if(socketfd < 0){
        perror("Socket failed\n");
        exit(1);
    }
    else cout << "Socket Creation successful.\n";
    
    // Server Information
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(portNo);
    inet_aton(serverIP, (struct in_addr *) &ServerAddress.sin_addr.s_addr);
    
    // Requesting for connection with the server
    if(connect(socketfd, (struct sockaddr *) &ServerAddress, addressLength) < 0){
        perror("Error while establishing connection\n");
        exit(1);
    }
    else cout << "Connection established with server.\n\n";
    
    // Communication : DNS Resolving System
    cout << "Time to send DNS request (Type | IP Adress/Domain Name) :-\n"; 
    bzero(bufferRequest, sizeof(bufferRequest)); // Assigning the buffer content to 0's
    fgets(bufferRequest, sizeof(bufferRequest), stdin); // Read the request message from the stdin
    int writeValue = write(socketfd, bufferRequest, strlen(bufferRequest)); // send that message to the server
    
    // writeValue, 0 : successful, -1 : failed
    if(writeValue < 0){
	perror("Error writing to socket\n");
	exit(1);
    }
    
    bzero(bufferResposne, sizeof(bufferResposne));
    // Read the information (message) sent by the server
    int readValue = read(socketfd, bufferResposne, sizeof(bufferResposne));
    
    if(readValue < 0){
	perror("Error reading from socket.\n");
	exit(1);
    }
    else cout << "Response from Server :-\n" << bufferResposne << endl;
    
    return 0; 
   
}
