#ifndef TCP_SOCKET
#define TCP_SOCKET

#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

class SocketTCP{
protected:
  int socket_fd;
  int port;
  std::string hostname;
public:
  SocketTCP(std::string dest_adr, int port);

  virtual ~SocketTCP();

  virtual int sendPackage(std::string package);

  virtual int receivePackage();

  int getSocketFd();

  std::string getHostName();

private:
  SocketTCP(){}
};

#endif // ! - TCP_SOCKET