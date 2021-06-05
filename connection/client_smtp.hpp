#ifndef CLIENT_SMTP
#define CLIENT_SMTP

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
#include <vector>
#include <map>

#include "ssl_socket.hpp"
#include "message.h"

class ClientSMTP{
private:
    SocketSSL* socket_ssl = NULL;
    std::string name;
    std::string password;

public:
    ClientSMTP(){}
    ClientSMTP(std::string name, std::string password);
    ~ClientSMTP();

    bool Autorithe();
    void setUserInfo(std::string, std::string);
    bool sendMessage(Message message);
    void closeSession();
};

#endif // ! - CLIENT_SMTP