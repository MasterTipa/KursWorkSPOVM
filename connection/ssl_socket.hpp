#ifndef SSL_SOCKET
#define SSL_SOCKET

#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "tcp_socket.hpp"

class SocketSSL{
protected:
    BIO *certbio = NULL;
    BIO *outbio = NULL;
    X509 *cert = NULL;
    X509_NAME *certname = NULL;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    SocketTCP* socket_tcp;
public:
  SocketSSL(std::string dest_adr, int port);

  ~SocketSSL();

  int sendPackage(std::string package);

  std::string receivePackage();
private:
  SocketSSL(){}
  //int createSocket(std::string hostname, int port);
};

#endif // ! - SSL_SOCKET