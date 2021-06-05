#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "tcp_socket.hpp"

SocketTCP::SocketTCP(std::string dest_adr, int port){
    /* ----------------------------------------------------------*
    * create_socket() creates the socket & TCP-connect to server *
    * ---------------------------------------------------------- */

    this->hostname = dest_adr;
    this->port = port;

    int sockfd;
    char *tmp_ptr = NULL;
    struct hostent *host;
    struct sockaddr_in dest_addr;   
    if ((host = gethostbyname(hostname.c_str())) == NULL ) {
      //BIO_printf(this->outbio, "Error: Cannot resolve hostname %s.\n",  hostname.c_str());
      std::cerr << "Error: Cannot resolve hostname " << this->hostname << std::endl;
      abort();
    }

    /* ---------------------------------------------------------- *
     * create the basic TCP socket                                *
     * ---------------------------------------------------------- */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);   
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr); 

    /* ---------------------------------------------------------- *
     * Zeroing the rest of the struct                             *
     * ---------------------------------------------------------- */

    memset(&(dest_addr.sin_zero), '\0', 8); 
    tmp_ptr = inet_ntoa(dest_addr.sin_addr);   

    /* ---------------------------------------------------------- *
     * Try to make the host connect here                          *
     * ---------------------------------------------------------- */

    if (connect(sockfd, (struct sockaddr *) &dest_addr,
                                sizeof(struct sockaddr)) == -1 ) {
      std::cerr << "Error: Cannot connect to host " << this->hostname << " [" << tmp_ptr
        << "] on port " << this->port << std::endl;
    }


    this->socket_fd = sockfd;
    return;
}

SocketTCP::~SocketTCP(){
    close(this->socket_fd);
}

int SocketTCP::sendPackage(std::string package){
    std::cerr << "Not supported yet" << std::endl;
    return -1;
}

int SocketTCP::receivePackage(){
    std::cerr << "Not supported yet" << std::endl;
    return -1;
}

int SocketTCP::getSocketFd(){
    return this->socket_fd;
}

std::string SocketTCP::getHostName(){
    return this->hostname;
}