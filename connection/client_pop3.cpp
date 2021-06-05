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
#include <regex>
#include <sstream>
#include <algorithm>
#include <map>

#include "client_pop3.hpp"

ClientPOP3::ClientPOP3(std::string name, std::string password){
    this->name = name;
    this->password = password;
}

ClientPOP3::~ClientPOP3(){
    delete this->socket_ssl;
}

void ClientPOP3::setUserInfo(std::string name, std::string password){
    this->name = name;
    this->password = password;
}

bool ClientPOP3::Autorithe(){
    if(this->name == std::string() || this->password == std::string()){
        return false;
    }
    try{
        std::string package;
        std::string buffer;
        buffer.clear();
        this->socket_ssl = new SocketSSL("pop.inbox.ru", 995);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){std::cout << "Here?" << std::endl;};
        std::cout << buffer;
        if(buffer.find("+OK") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "USER "+ this->name + "\r\n";
        std::cout << "ClientPOP3 : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("+OK") == std::string::npos){
            return false;
        }
        buffer.clear();
        package = "PASS "+ this->password +"\r\n";
        std::cout << "ClientPOP3 : " << package;
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
        std::cout << buffer;
        if(buffer.find("+OK") == std::string::npos){
            return false;
        }
        buffer.clear();
    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }

    return true;
}

std::map<size_t, std::string> ClientPOP3::getMessagesHeader(){
    
    std::map<size_t, std::string> message_headers_array;

    try{
        std::string package;
        std::string buffer = "";
        package = "LIST\r\n";
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n.\r\n") == std::string::npos){};
        if(buffer.find("+OK") == std::string::npos){
            return std::map<size_t, std::string>();
        }

        std::regex ex("( ?[0-9]+ ?)");
        std::regex_iterator<std::string::iterator> regex_iterator = std::regex_iterator<std::string::iterator>(buffer.begin(), buffer.end(), ex);
        
        std::stringstream ss;
        ss << (*regex_iterator)[0];
        size_t number_of_msgs;
        ss >> number_of_msgs;

        ex = std::regex("([0-9]+) ([0-9]+)");
        regex_iterator = std::regex_iterator<std::string::iterator>(buffer.begin(), buffer.end(), ex);

        std::for_each(regex_iterator, std::regex_iterator<std::string::iterator>(), [&](auto & res){
            std::stringstream ss;
            std::string buffer = "";
            size_t id = 0;
            std::string id_str = res[1];
            ss << res[1];
            ss >> id;
            std::string package = std::string("TOP ") + id_str +" 2"+std::string("\r\n");
            std::cout << "ClientPOP3 : " << package << std::endl; 
            this->socket_ssl->sendPackage(package);
            while((buffer += this->socket_ssl->receivePackage()).find("\r\n.\r\n") == std::string::npos){};
            std::cout << buffer;
            message_headers_array.insert({id, buffer});
        });

    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }
    
    return message_headers_array;
}

std::pair<size_t, std::string> ClientPOP3::getMessageById(size_t id){
    std::string buffer = "";
    std::string id_str = std::to_string(id);
    std::string package = std::string("RETR ") + id_str + std::string("\r\n");
    this->socket_ssl->sendPackage(package);
    while((buffer += this->socket_ssl->receivePackage()).find("\r\n.\r\n") == std::string::npos){};
    if(buffer.find("+OK") == std::string::npos){
        return std::pair<size_t, std::string>();
    }
    return std::pair<size_t, std::string>(id, buffer);
}

std::map<size_t, std::string> ClientPOP3::getMessages(){

    std::map<size_t, std::string> message_array;

    try{
        std::string package;
        std::string buffer = "";
        package = "LIST\r\n";
        this->socket_ssl->sendPackage(package);
        while((buffer += this->socket_ssl->receivePackage()).find("\r\n.\r\n") == std::string::npos){
        //std::cout<<"While cycle"<<std::endl;
        };
        if(buffer.find("+OK") == std::string::npos){
            //std::cout<<"Return map"<<std::endl;
            return std::map<size_t, std::string>();
        }
        std::regex ex("( ?[0-9]+ ?)");
        std::regex_iterator<std::string::iterator> regex_iterator = std::regex_iterator<std::string::iterator>(buffer.begin(), buffer.end(), ex);
        //std::cout<<"Return map after If"<<std::endl;
        
        std::stringstream ss;
        ss << (*regex_iterator)[0];
        size_t number_of_msgs;
        ss >> number_of_msgs;

        ex = std::regex("([0-9]+) ([0-9]+)");
        regex_iterator = std::regex_iterator<std::string::iterator>(buffer.begin(), buffer.end(), ex);
            //std::cout<<buffer<<std::endl;
        std::for_each(regex_iterator, std::regex_iterator<std::string::iterator>(), [&](auto & res){
            std::stringstream ss;
            std::string buffer = "";
            size_t id = 0;
            std::string id_str = res[1];
            ss << res[1];
            ss >> id;
            std::string package = std::string("RETR ") + id_str + std::string("\r\n");
            //std::cout<<"Socket SSL"<<std::endl;
        
            this->socket_ssl->sendPackage(package);
            while((buffer += this->socket_ssl->receivePackage()).find("\r\n.\r\n") == std::string::npos){
                //std::cout<<"Socket While"<<std::endl;
                
            };
            message_array.insert({id, buffer});
        });

    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }
    
    return message_array;
}

bool ClientPOP3::deleteMessage(size_t id){
    auto str_id = std::to_string(id);
    std::string package = "DELE "+ str_id + "\r\n";
    std::string buffer;
    this->socket_ssl->sendPackage(package);
    while((buffer += this->socket_ssl->receivePackage()).find("\r\n") == std::string::npos){};
    std::cout << buffer;
    
    return (buffer.find("+OK") == std::string::npos) ? false : true;
}

std::string ClientPOP3::getUserEmail(){
    return this->name;
}

void ClientPOP3::closeSession(){
    try{
        std::string package;
        package = "QUIT\r\n";
        std::cout << "ClientPOP3 : " << package;
        this->socket_ssl->sendPackage(package);
    }
    catch(std::string e){
        std::cerr << e << std::endl;
    }
}