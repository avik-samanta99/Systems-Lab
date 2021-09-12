**Steps followed to implement Multi-stage DNS Resolving System using Client-Server socket programming-**

1. We have implemented 3 programs, namely, client.c, ProxyServer.c and DNSServer.c

2. Open 3 terminals (where one terminal will behave like a client, second one will behave like proxy server and third terminal will behave like a DNS server).

3. Compile 3 programs as follows.

➢ **g++ -o dns_server DNSServer.cpp**

➢ **g++ -o proxy_server ProxyServer.cpp**

➢ **g++ -o client Client.cpp**

4. Now, there will be 3 executable files dns_server, proxy_server, and client, that will be created.

5. While running DNSServer.c we have to input the DNS server's port, running ProxyServer.c requires us to input the Proxy Server's port, and while running client.c, we have to input the IP address and the Port of the proxy server(we have considered all the 3 programs to run on the local host, i.e. 127.0.0.1).

6. To execute the 3 programs, we need to run the following commands, in the respective windows:

➢ **$ ./dns_server 4201**

➢ **$ ./proxy_server 4200**

➢ **$ ./client 127.0.0.1 4200**

7. This assigns the port number 4200 to the dns server, and 4201 to the proxy server, and tells the client program, the IP address(127.0.0.1) and the port number(4201) of the proxy server.

8. Request can be of two types.(Type1 & Type 2)

➢ **Type 1** Message field contains Domain Name and requests for corresponding IP address.

➢ **Type 2** Message field contains IP address and request for the corresponding Domain Name.

9. If the proxy cache doesn't contain an entry corresponding to the requested domain name/IP address, then the proxy server will ask the user to input the DNS server's IP address and port number, which in our example are 127.0.0.1 and 4201 respectivly. After establishing a connection with the DNS server, the proxy server will now forward the request to the DNS server, which will check if there is an entry corresponding to the request. If there doesn't exist any entry, the DNS server sends a message to the proxy server saying that it can't service the request, which the proxy server will forward to the client program.

10. If the proxy cache contains an entry corresponding to the requested domain name/IP address, the proxy server program sends back the data to the client program.

11. If a corresponding entry exists, the DNS server will send the required data to the proxy server, which will now update it's proxy cache using the FIFO algorithm, and then it forwards the data to the client program.

12. We have implemented the proxy cache and the DNS database as text programs with the names serverCache.txt and DNSDatabase.txt.

