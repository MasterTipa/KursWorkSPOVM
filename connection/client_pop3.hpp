#ifndef CLIENT_POP3
#define CLIENT_POP3

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

class ClientPOP3{
private:
    SocketSSL* socket_ssl = NULL;
    std::string name;
    std::string password;

public:
    ClientPOP3(){}
    ClientPOP3(std::string name, std::string password);
    ~ClientPOP3();

    bool Autorithe();
    void setUserInfo(std::string, std::string);
    std::map<size_t, std::string> getMessages();
    std::map<size_t, std::string> getMessagesHeader();
    bool deleteMessage(size_t);
    std::pair<size_t, std::string> getMessageById(size_t);
    void closeSession();
    std::string getUserEmail();
    
};

#endif // ! - CLIENT_POP3