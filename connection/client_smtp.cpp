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

#include "client_smtp.hpp"
#include "message.h"

ClientSMTP::ClientSMTP(std::string name, std::string password){
    this->name = name;
    this->password = password;
}

ClientSMTP::~ClientSMTP(){
    delete this->socket_ssl;
}

bool ClientSMTP::Autorithe(){
    if(this->name == std::string() || this->password == std::string()){
        return false;
    }
    try{
        std::string package;
        std::string buffer;
        buffer.clear();
        this->socket_ssl = new SocketSSL("smtp.inbox.ru", 465);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("220") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "EHLO LoliMailerUser\r\n";
        std::cout << "ClientSMTP : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("250") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "AUTH LOGIN\r\n";
        std::cout << "ClientSMTP : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("334") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = this->name + "\r\n";
        std::cout << "ClientSMTP : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("334") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = this->password + "\r\n";
        std::cout << "ClientSMTP : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("235") == std::string::npos){
            return false;
        }
        buffer.clear();
    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }

    return true;
}

void ClientSMTP::setUserInfo(std::string name, std::string password){
    this->name = name;
    this->password = password;
}

bool ClientSMTP::sendMessage(Message message){
    try{
        std::string package;
        std::string buffer;
        buffer.clear();
        package = "MAIL FROM:"+ message.getSenderContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << "ClientSMTP: " << package;
        std::cout << buffer;
        if(buffer.find("250") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "RCPT TO:" + message.getRecipientContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << "ClientSMTP: " << package;
        std::cout << buffer;
        if(buffer.find("250") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "DATA\r\n";
        //while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        //std::cout << buffer;
        //if(buffer.find("354") == std::string::npos){
        //    return false;
        //}
        //buffer.clear();
        this->socket_ssl->sendPackage(package);
        std::cout << "ClientSMTP: " << package;
        package = "FROM:" + message.getSenderContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        std::cout << "ClientSMTP: " << package;
        package = "TO:" + message.getRecipientContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        std::cout << "ClientSMTP: " << package;
        package = "SUBJECT:" + message.getSubjectContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        std::cout << "ClientSMTP: " << package;
        package = "Content-Type:" + message.getContentTypeContext() + "\r\n";
        this->socket_ssl->sendPackage(package);
        std::cout << "ClientSMTP: " << package;
        this->socket_ssl->sendPackage(message.getDataContext());
        std::cout << "ClientSMTP: " << message.getDataContext() << std::endl;
        package = "\r\n.\r\n";
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        buffer.clear();
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("250") == std::string::npos){
            return false;
        }
        buffer.clear();
    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }

    return true;
}


void ClientSMTP::closeSession(){
    try{
        std::string package;
        package = "QUIT\r\n";
        std::cout << "ClientSMTP : " << package;
        this->socket_ssl->sendPackage(package);
    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }
}