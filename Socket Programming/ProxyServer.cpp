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

// proxy server communicating with the DNS Server
string communicateDNS(char buffer[]){

    int socketfd, newSocketConnected; 
    struct sockaddr_in DNSServerAddress;
    int addressLength = sizeof(DNSServerAddress);
    char bufferDNSRequest[1024], bufferDNSResponse[1024], bufferResponse[1024]; 
    
    // ask for DNS Server IP
    char DNSIP[100];
    cout << "Enter the IP Address of the DNS Server : ";
    cin >> DNSIP;
    
    // ask for DNS Server port no
    int DNSPortNo;
    cout << "Enter the port no for communication with DNS Server : ";
    cin >> DNSPortNo;
    
    // create the socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketfd < 0){
        perror("Socket failed\n");
        exit(1);
    }
    else cout << "Socket Creation successful.\n";
    
    // feed the DNS Server data
    DNSServerAddress.sin_family = AF_INET;
    DNSServerAddress.sin_port = htons(DNSPortNo);
    inet_aton(DNSIP, (struct in_addr *) &DNSServerAddress.sin_addr.s_addr);
    
    // connect with the DNS server
    if(connect(socketfd, (struct sockaddr *) &DNSServerAddress, addressLength) < 0){
        perror("Error while establishing connection with DNS.\n");
        exit(1);
    }
    else cout << "Connection established with DNS server.\n\n";
    
    // forward the message from the client to the DNS server
    cout << "Time to forward the DNS request...\n"; 
    bzero(bufferDNSRequest, sizeof(bufferDNSRequest));
    strcpy(bufferDNSRequest, buffer);
    int writeValue = write(socketfd, bufferDNSRequest, strlen(bufferDNSRequest));
    
    if(writeValue < 0){
	perror("Error writing to socket\n");
	exit(1);
    }
    
    bzero(bufferDNSResponse, sizeof(bufferDNSResponse));
    // read the response from the DNS Server
    int readValue = read(socketfd, bufferDNSResponse, sizeof(bufferDNSResponse));
    
    if(readValue < 0){
	perror("Error reading from socket.\n");
	exit(1);
    }
    else cout << "Response from DNS Server : " << bufferDNSResponse << endl;
    
    // if DNS responded with actual information (ip/domain)
    if(bufferDNSResponse[0] == '3'){
    
        bzero(bufferResponse, sizeof(bufferResponse));
        strcpy(bufferResponse, bufferDNSResponse + 2);
        
        string ip, domain, line;
        int countEntries = 0;
        
        if(buffer[0] == '1'){ // domain -> ip
            domain = getActualDomain(buffer);
            ip = bufferResponse;
        }
        
        else if(buffer[0] == '2'){ // ip -> domain
            ip = buffer + 2;
            ip.erase(remove(ip.begin(), ip.end(), '\n'), ip.end());
            domain = bufferResponse;
        }
        
        // update the Proxy server cache
        ifstream fp("serverCache.txt");
        while(getline(fp, line)){
            countEntries ++; // count the entries in the local cache
        }
        if(countEntries < 3){ // if the cache is not full yet
            ofstream fp("serverCache.txt", ios::app);
            fp << ip << " " << domain << endl;
            fp.close();
        }
        else{ // if the cache is full, insert the new information in FIFO manner
            ofstream outptr("tempCache.txt");
            ifstream inptr("serverCache.txt");
            string line;
            getline(inptr, line);
            while(getline(inptr, line)){
                outptr << line << endl;
            }
            outptr << ip << " " << domain << endl;
            inptr.close();
            outptr.close();
            remove("serverCache.txt");
            rename("tempCache.txt", "serverCache.txt");
        }
        
        // return the response
        string response;
        if(buffer[0] == '1'){
            response = "The IP address for the domain " + domain + " is : " + ip;
        }
        else if(buffer[0] == '2'){
            response = "The Domain name for the IP Address " + ip + " is : " + domain;
        }
        return response;
    }
    
    // when even DNS database does not contain the required information
    else if(bufferDNSResponse[0] == '4'){
        string response = "Error! Entry not found in the Database!";
        return response;
    }
    
    // invalid response
    else{
        string response = "Invalid message returned by the DNS Server...";
        return response;
    }
    
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
    
    // file descriptor, 0 : success, -1 : failure
    if(socketfd < 0){
        perror("Socket failed\n");
        exit(1);
    }
    
    // SockOpt, to reuse the address and port and to avoid the error like, "address already in use"
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("Setsockopt"); 
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
			// read the request (message) from the client
			int readValue = read(newSocketConnected, buffer, sizeof(buffer));
			
			if(readValue < 0){
			perror("Error reading from socket\n");
			exit(1);
			}
			
			cout << "Request from Client : " << buffer;
			
			// if the request is for domain name -> ip address
			if(buffer[0] == '1'){
				string actualDomain = getActualDomain(buffer), requiredIP;
				ifstream fp("serverCache.txt");
				string ip, domain;
				bool flag = false;
				while((fp >> ip)){
					fp >> domain;
					// checking the proxy server cache, 
					if(actualDomain == domain){
					    requiredIP = ip;
					    flag = true;
					    break;
					}
				}
				fp.close();
				if(flag){ // cache hit
					string response = "The IP address for the requested domain " + actualDomain + " is : " + requiredIP;
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				else{ // cache miss
					cout << "Cache miss. Connecting with DNS Server...\n";
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, communicateDNS(buffer).c_str()); // communicate with DNS Server
				}
				
				// sending the response back to the client
				int writeValue = write(newSocketConnected, bufferResponse, strlen(bufferResponse));
					
				if(writeValue < 0){
				    perror("Error writing to the socket.\n");
				    exit(1);
				}
			}
			
			// if the request if for ip address -> domain name
			else if(buffer[0] == '2'){
				string IP = buffer + 2, requiredDomain;
				IP.erase(remove(IP.begin(), IP.end(), '\n'), IP.end());
				ifstream fp("serverCache.txt");
				string ip, domain;
				bool flag = false;
				while((fp >> ip)){
					fp >> domain;
					// checking for the local cache
					if(IP == ip){
					    requiredDomain = domain;
					    flag = true;
					    break;
					}
				}
				fp.close();
				if(flag){ // cache hit
					string response = "The domain for the requested IP address " + IP + " is : " + requiredDomain;
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, response.c_str());
				}
				else{ // cache miss
					cout << "Cache miss. Connecting with DNS Server...\n";
					bzero(bufferResponse, sizeof(bufferResponse));
					strcpy(bufferResponse, communicateDNS(buffer).c_str()); // communicate with the DNS Server
				}
				
				// sending the response back to the client
				int writeValue = write(newSocketConnected, bufferResponse, strlen(bufferResponse));
					
				if(writeValue < 0){
				    perror("Error writing to the socket.\n");
				    exit(1);
				}
				
			}
			
			// if the request is invalid
			else{
				string response = "Invalid request! Try again...\n";
				bzero(bufferResponse, sizeof(bufferResponse));
				strcpy(bufferResponse, response.c_str());
				int writeValue = write(newSocketConnected, bufferResponse, strlen(bufferResponse));
				
				if(writeValue < 0){
					    perror("Error writing to the socket.\n");
					    exit(1);
				}
			}
			
		}
		
		// parent process, make it ready for taking more requests
		else{
		    goto parentProcess;
		}
    
    close(socketfd);
    
    return 0; 
   
}
