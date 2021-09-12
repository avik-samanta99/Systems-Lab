#include<bits/stdc++.h>
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

// this function extracts actual domain name from the request message
string getActualDomain(char buffer[]){
    string actualDomain;
    if(buffer[2] == 'w' and buffer[3] == 'w' and buffer[4] == 'w' and buffer[5] == '.')
        actualDomain = buffer + 6;
    else actualDomain = buffer + 2;
    actualDomain.erase(remove(actualDomain.begin(), actualDomain.end(), '\n'), actualDomain.end());
    return actualDomain;
}

int main(int argc, char *argv[]){
    
    // We need two arguments at the command-line, Filename and Port number
    if(argc < 2){
        cout << "Error! Not enough information provided at the command line.\n";
        exit(0);
    }
    
    // Some variable which we will need later in our programme
    int socketfd; // File descriptor for the socket (0 : if created, -1 : if failed)
    int  newSocketConnected; // Newly created socket descriptor after requesting for connection (0 : if created, -1 : if failed)
    struct sockaddr_in ServerAddress, ClientAddress; // Server (Proxy server) and client address
    int addressLength = sizeof(ServerAddress), opt; // Lengtho or size of the address
    char buffer[1024], bufferResponse[1024]; // buffer for the requested and response message
    
    // Port number from the command line
    int portNo = stoi(argv[1]);
    
    // Creating the socket with AF_INET (IPv4), SOCK_STREAM (TCP), Default Internet Protocol (0)
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketfd < 0){
        perror("Socket failed\n");
        exit(1);
    }
    
    // SockOpt, to reuse the address and port and to avoid the error like, "address already in use"
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt\n"); 
        exit(1); 
    }
    
    // Server Information
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(portNo);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    
    // binding the socket with the address and port no
    if(bind(socketfd, (struct sockaddr *) &ServerAddress, addressLength) < 0){
        perror("Bind Failed\n");
        exit(1);
    }

    // going to passive mode and listen for client requests
    if(listen(socketfd, 5) < 0){
        perror("Listen failed\n");
        exit(1);
    }
    
	parentProcess:

        // accepting and establishing connection from the client
		newSocketConnected = accept(socketfd, (struct sockaddr *) &ClientAddress, (socklen_t *) &addressLength);
		
		// 0 : successful, -1 : failed
		if(newSocketConnected < 0){
		    perror("Connection Error\n");
		    exit(0);
		}
		
		// creating a new thread for communication with the client
		int pid = fork();
		
		// inside the newly created (child) process
		if(pid == 0){
		
			bzero(buffer, sizeof(buffer));
			int readValue = read(newSocketConnected, buffer, sizeof(buffer));
			
			if(readValue < 0){
			perror("Error reading from socket\n");
			exit(1);
			}
			
			cout << "Request message from proxy server : " << buffer;
			
			if(buffer[0] == '1'){ // domain -> ip
			
				string actualDomain = getActualDomain(buffer), requiredIP;
				
				ifstream fp("DNSDatabase.txt");
				string ip, domain;
				bool flag = false;
				while((fp >> ip)){
					fp >> domain;
					// checking from the database
					if(actualDomain == domain){
					    requiredIP = ip;
					    flag = true;
					    break;
					}
				}
				fp.close();
				if(flag){ // found in the database
					string response = "3 " + requiredIP;
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				else{ // information not there in database
					string response = "4 Entry not found in the Database";
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				
				int writeValue = write(newSocketConnected, bufferResponse, strlen(bufferResponse));
					
				if(writeValue < 0){
				    perror("Error writing to the socket.\n");
				    exit(1);
				}
			}
			
			else if(buffer[0] == '2'){ // ip -> domain
			
				string IP = buffer + 2, requiredDomain;
				IP.erase(remove(IP.begin(), IP.end(), '\n'), IP.end());
				ifstream fp("DNSDatabase.txt");
				string ip, domain;
				bool flag = false;
				while((fp >> ip)){
					fp >> domain;
					// cheching from the database
					if(IP == ip){
					    requiredDomain = domain;
					    flag = true;
					    break;
					}
				}
				fp.close();
				if(flag){ // found
					string response = "3 " + requiredDomain;
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				else{ // not found
					string response = "4 Entry not found in the Database";
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				
				int writeValue = write(newSocketConnected, bufferResponse, strlen(bufferResponse));
					
				if(writeValue < 0){
				    perror("Error writing to the socket.\n");
				    exit(1);
				}
				
			}
		}
		
		else{
		    goto parentProcess;
		}	
		
    
    return 0; 
   
}
