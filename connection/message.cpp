#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <regex>
#include <sstream>
#include <thread>
#include <future>
#include <pthread.h>

#include "message.h"
#include "base64.h"

template <typename T>
class Thread_argument{
public:
    Message* object;
    std::string argument;
    T result;
};

bool Message::Parse(std::string & raw_data){
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    Thread_argument<std::string> arg_sender;
    arg_sender.object = this;
    arg_sender.argument = raw_data;

    pthread_t sender_thread;
    pthread_create(&sender_thread, &attr, [](void* argument) -> void* {
        Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
        arg->result = arg->object->getSender(arg->argument);
        arg->argument.clear();
        return nullptr;
    }, (void*)&arg_sender);

    Thread_argument<std::string> arg_from;
    arg_from.object = this;
    arg_from.argument = raw_data;

    pthread_t from_thread;
    pthread_create(&from_thread, &attr, [](void* argument) -> void* {
        Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
        arg->result = arg->object->getFromFiled(arg->argument);
        arg->argument.clear();
        return nullptr;
    }, (void*)&arg_from);

    Thread_argument<std::string> arg_recipient;
    arg_recipient.object = this;
    arg_recipient.argument = raw_data;

    pthread_t recipient_thread;
    pthread_create(&recipient_thread, &attr, [](void* argument) -> void* {
        Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
        arg->result = arg->object->getRecipient(arg->argument);
        arg->argument.clear();
        return nullptr;
    }, (void*)&arg_recipient);

    Thread_argument<std::string> arg_subject;
    arg_subject.object = this;
    arg_subject.argument = raw_data;

    pthread_t subject_thread;
    pthread_create(&subject_thread, &attr, [](void* argument) -> void* {
        Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
        arg->result = arg->object->getSubject(arg->argument);
        arg->argument.clear();
        return nullptr;
    }, (void*)&arg_subject);


    Thread_argument<std::vector<std::string>> arg_content_type;
    arg_content_type.object = this;
    arg_content_type.argument = raw_data;

    pthread_t content_type_thread;
    pthread_create(&content_type_thread, &attr, [](void* argument) -> void* {
        Thread_argument<std::vector<std::string>>* arg = reinterpret_cast<Thread_argument<std::vector<std::string>>*>(argument);
        arg->result = arg->object->getContentType(arg->argument);
        arg->argument.clear();
        return nullptr;
    }, (void*)&arg_content_type);

    Thread_argument<std::string> arg_data;
    arg_data.object = this;
    arg_data.argument = raw_data;
    pthread_t data_thread;
    pthread_join(content_type_thread, nullptr);

    std::vector<std::string> content_type = arg_content_type.result;
    if(content_type.size() > 1){
        this->content_type = content_type[0];
        this->boundary = content_type[1];
    }
    else{
        this->content_type = content_type[0];
    }

    if(this->content_type == mime::type_constants::mime_related || this->content_type == mime::type_constants::mime_alternative 
        || this->content_type == mime::type_constants::mime_mixed){
            try{
                pthread_create(&data_thread, &attr, [](void* argument) -> void* {
                Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
                    arg->result = arg->object->getContentMultipart(arg->argument);
                    arg->argument.clear();
                    return nullptr;
                }, (void*)&arg_data);
            }
            catch(std::string e){
                std::cout << e << std::endl;
            }
        }
    else{
        pthread_create(&data_thread, &attr, [](void* argument) -> void* {
            Thread_argument<std::string>* arg = reinterpret_cast<Thread_argument<std::string>*>(argument);
            arg->result = arg->object->getContentMultipart(arg->argument);
            arg->argument.clear();
            return nullptr;
        }, (void*)&arg_data);
    }

    pthread_join(sender_thread, nullptr);
    pthread_join(from_thread, nullptr);
    pthread_join(recipient_thread, nullptr);
    pthread_join(subject_thread, nullptr);
    pthread_join(data_thread, nullptr);

    this->sender = arg_sender.result;
    this->from = arg_from.result;
    this->recipient = arg_recipient.result;
    this->subject = arg_subject.result;
    this->data = arg_data.result;

    return true;

}

std::string Message::getContentMultipart(std::string & raw_data){
    std::string buffer = this->getEncodedBlock(raw_data);

    std::string encoding_type = this->getEncodingType(buffer);
    if(encoding_type == mime::encoding_type_constants::mime_base64){
        return this->base64Decode(this->getData(buffer));
    }

    if(encoding_type == mime::encoding_type_constants::mime_quoted_printable){
        return this->QPDecode(this->getData(buffer));
    }
    if(encoding_type == std::string()){
        return  this->getData(buffer);
    }

    return std::string();
}

std::string Message::getContentSinglepart(std::string & raw_data){
    std::string encoding_type = this->getEncodingType(raw_data);
    if(encoding_type == mime::encoding_type_constants::mime_base64){
        return this->base64Decode(this->getData(raw_data));
    }

    if(encoding_type == mime::encoding_type_constants::mime_quoted_printable){
        return this->QPDecode(this->getData(raw_data));
    }

    return std::string();
}

std::string Message::getEncodedBlock(std::string & raw_data){
    std::string buffer;
    buffer.clear();
    std::string temp_boundary = "--" + this->boundary;
    std::string::iterator iter = std::search(raw_data.begin(), raw_data.end(), temp_boundary.begin(), temp_boundary.end());
    iter+=temp_boundary.length();
    std::string::iterator temp = std::search(iter, raw_data.end(), temp_boundary.begin(), temp_boundary.end());
    buffer.resize(std::distance(iter, temp));
    std::copy(iter, temp, buffer.begin());
    std::string content_type = this->getContentType(buffer)[0];
    if(content_type != mime::type_constants::mime_html){
        iter = temp + temp_boundary.length();
        temp = std::search(iter, raw_data.end(), temp_boundary.begin(), temp_boundary.end());
        buffer.resize(std::distance(iter, temp));
        std::copy(iter, temp, buffer.begin());
    }

    return buffer;
}

std::string Message::getEncodingType(std::string & raw_data){
    std::regex expression("Content-Transfer-Encoding:[ \r\n\t]+([a-zA-Z0-9-]+)");
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(), raw_data.end(), expression);
    return (std::distance((*regex_iterator).begin(), (*regex_iterator).end()) > 1) ? (*regex_iterator)[1] : std::string();
}

std::string Message::getData(std::string & raw_data){
    std::string buffer;
    std::string boundary = "\r\n";
    buffer.clear();
    std::string::iterator iterator_begin = raw_data.begin();
    std::string::iterator temp = raw_data.begin();
    std::string::iterator iterator_end = raw_data.begin();


    iterator_end = std::search(iterator_begin, raw_data.end(), boundary.begin(), boundary.end());
    iterator_begin = iterator_end + boundary.length();
    iterator_end = std::search(iterator_begin, raw_data.end(), boundary.begin(), boundary.end());

    do{
        buffer.resize(std::distance(iterator_begin, iterator_end));
        std::copy(iterator_begin, iterator_end, buffer.begin());
        iterator_begin = iterator_end + boundary.length();
        iterator_end = std::search(iterator_begin, raw_data.end(), boundary.begin(), boundary.end());
    }while(buffer != std::string());


    buffer.resize(std::distance(iterator_begin, raw_data.end() - boundary.length()));
    std::copy(iterator_begin, raw_data.end() - boundary.length(), buffer.begin());

    
    
    return buffer;
}

std::vector<std::string> Message::getContentType(std::string & raw_data){
    std::vector<std::string> content_type;
    std::regex expression("Content-Type: ([a-zA-Z0-9/]+);[ \r\n\t]+(.+)", std::regex_constants::ECMAScript);
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(),
        raw_data.end(), expression);

    content_type.push_back(((*regex_iterator)[1]));


    if(content_type[0] == mime::type_constants::mime_related || content_type[0] == mime::type_constants::mime_alternative 
        || content_type[0] == mime::type_constants::mime_mixed){
            std::string temp = (*regex_iterator)[2];
            std::regex boundary_ex("boundary=\"(.+)\"");
            content_type.push_back(((*(std::regex_iterator<std::string::iterator>(temp.begin(), temp.end(), boundary_ex)))[1]));    
        }

    return content_type;
}

std::string Message::getSender(std::string & raw_data){
    std::regex expression("Return-path:[ \r\n\t]+<(.+)>");
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(), raw_data.end(), expression);

    return ((*regex_iterator).length() > 1) ? (*regex_iterator)[1] : std::string();
}

std::string Message::getFromFiled(std::string & raw_data){
    std::regex expression("[\r\n]From:[ \r\n\t]+(.+)");
    std::string str_exp("From: ");
    std::string crlf("\r\n");
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(), raw_data.end(), expression);

    std::string from_field = ((*regex_iterator).length() > 1) ? (*regex_iterator)[1] : std::string();


    if(from_field.find(mime::charset_types::utf8) != std::string::npos){
        from_field.clear();
        std::string end_encoded_string = "?=";
        std::string start_encoded_string = "=?";
        auto from_field_iter = std::search(raw_data.begin(), raw_data.end(), str_exp.begin(), str_exp.end());
        from_field_iter+=str_exp.length();
        from_field_iter = std::search(from_field_iter, raw_data.end(), start_encoded_string.begin(), start_encoded_string.end());
        auto end_buffer = std::search(from_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        std::string buffer;
        buffer.clear();
        do{
            buffer.clear();
            std::copy(from_field_iter, end_buffer, std::back_inserter(buffer));
            from_field += buffer;
            from_field_iter = end_buffer + crlf.length();
            while(*from_field_iter == ' ')
                ++from_field_iter;
            end_buffer = std::search(from_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        }while(buffer.find(mime::charset_types::utf8) != std::string::npos && buffer[0] == '=');

        from_field.erase(from_field.find(buffer), buffer.length());
        std::string::iterator filed_iterator = std::search(from_field.begin(), from_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
        filed_iterator+=mime::charset_types::utf8.length() + 1;
        if(*filed_iterator == 'b' || *filed_iterator == 'B'){
            filed_iterator += 2;
            std::string buffer;
            std::string::iterator temp = std::search(filed_iterator, from_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + 2;
                filed_iterator = std::search(filed_iterator, from_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 1;
                temp = std::search(filed_iterator, from_field.end(), end_encoded_string.begin(), end_encoded_string.end());
                temp+=2;
            }while(filed_iterator < from_field.end() && temp < from_field.end());

            return this->base64Decode(buffer);
        }
        if(*filed_iterator == 'Q'){
            filed_iterator += 2;
            std::string buffer;
            std::string::iterator temp = std::search(filed_iterator, from_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                //buffer.resize(buffer.length() + std::distance(filed_iterator, temp));
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + end_encoded_string.length();
                filed_iterator = std::search(filed_iterator, from_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 1;
                temp = std::search(filed_iterator, from_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            }while(filed_iterator < from_field.end() && temp < from_field.end());

            return this->QPDecode(buffer);
        }
    }

    return from_field;
}

std::string Message::getRecipient(std::string & raw_data){
    std::regex expression("[\r\n]To:[ \r\n\t]+(.+)");
    std::string str_exp("To:");
    std::string crlf("\r\n");
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(), raw_data.end(), expression);

    std::string to_field = ((*regex_iterator).length() > 1) ? (*regex_iterator)[1] : std::string();


     if(to_field.find(mime::charset_types::utf8) != std::string::npos){
        to_field.clear();
        std::string end_encoded_string = "?=";
        std::string start_encoded_string = "=?";
        auto to_field_iter = std::search(raw_data.begin(), raw_data.end(), str_exp.begin(), str_exp.end());
        to_field_iter+=str_exp.length();
        to_field_iter = std::search(to_field_iter, raw_data.end(), start_encoded_string.begin(), start_encoded_string.end());
        auto end_buffer = std::search(to_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        std::string buffer;
        buffer.clear();
        do{
            buffer.clear();
            std::copy(to_field_iter, end_buffer, std::back_inserter(buffer));
            to_field += buffer;
            to_field_iter = end_buffer + crlf.length();
            while(*to_field_iter == ' ')
                ++to_field_iter;
            end_buffer = std::search(to_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        }while(buffer.find(mime::charset_types::utf8) != std::string::npos && buffer[0] == '=');

        to_field.erase(to_field.find(buffer), buffer.length());
        std::string::iterator filed_iterator = std::search(to_field.begin(), to_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
        filed_iterator+=mime::charset_types::utf8.length() + 1;
        if(*filed_iterator == 'b' || *filed_iterator == 'B'){
            filed_iterator += 2;
            std::string buffer;
            std::string::iterator temp = std::search(filed_iterator, to_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                //buffer.resize(buffer.length() + std::distance(filed_iterator, temp));
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + end_encoded_string.length();
                filed_iterator = std::search(filed_iterator, to_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 1;
                temp = std::search(filed_iterator, to_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            }while(filed_iterator < to_field.end() && temp < to_field.end());

            return this->base64Decode(buffer);
        }
        if(*filed_iterator == 'Q'){
            filed_iterator += 2;
            std::string buffer;
            std::string::iterator temp = std::search(filed_iterator, to_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                //buffer.resize(buffer.length() + std::distance(filed_iterator, temp));
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + end_encoded_string.length();
                filed_iterator = std::search(filed_iterator, to_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 1;
                temp = std::search(filed_iterator, to_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            }while(filed_iterator < to_field.end() && temp < to_field.end());

            return this->QPDecode(buffer);
        }
    }

    return to_field;
}

std::string Message::getSubject(std::string & raw_data){
    std::regex expression("[\r\n]Subject:[ \r\n\t]+(.+)");
    std::string str_exp("Subject:");
    std::string crlf("\r\n");
    auto regex_iterator = std::regex_iterator<std::string::iterator>(raw_data.begin(), raw_data.end(), expression);

    std::string subject_field = ((*regex_iterator).length() > 1) ? (*regex_iterator)[1] : std::string();


     if(subject_field.find(mime::charset_types::utf8) != std::string::npos){
        subject_field.clear();
        std::string end_encoded_string = "?=";
        std::string start_encoded_string = "=?";
        auto subject_field_iter = std::search(raw_data.begin(), raw_data.end(), str_exp.begin(), str_exp.end());
        subject_field_iter+=str_exp.length();
        subject_field_iter = std::search(subject_field_iter, raw_data.end(), start_encoded_string.begin(), start_encoded_string.end());
        auto end_buffer = std::search(subject_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        std::string buffer;
        buffer.clear();
        do{
            buffer.clear();
            std::copy(subject_field_iter, end_buffer, std::back_inserter(buffer));
            subject_field += buffer;
            subject_field_iter = end_buffer + crlf.length();
            while(*subject_field_iter == ' ')
                ++subject_field_iter;
            end_buffer = std::search(subject_field_iter, raw_data.end(), crlf.begin(), crlf.end());
        }while(buffer.find(mime::charset_types::utf8) != std::string::npos && buffer[0] == '=');

        subject_field.erase(subject_field.find(buffer), buffer.length());

        std::string::iterator filed_iterator = std::search(subject_field.begin(), subject_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
        filed_iterator+=mime::charset_types::utf8.length() + 1;
        if(*filed_iterator == 'b' || *filed_iterator == 'B'){
            filed_iterator += 2;
            buffer.clear();
            std::string::iterator temp = std::search(filed_iterator, subject_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + end_encoded_string.length();
                filed_iterator = std::search(filed_iterator, subject_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 3;
                temp = std::search(filed_iterator, subject_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            }while(temp != subject_field.end());


            return this->base64Decode(buffer);
        }
        if(*filed_iterator == 'Q'){
            filed_iterator += 2;
            buffer.clear();
            std::string::iterator temp = std::search(filed_iterator, subject_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            do{
                std::copy(filed_iterator, temp, std::back_inserter(buffer));
                filed_iterator = temp + end_encoded_string.length();
                filed_iterator = std::search(filed_iterator, subject_field.end(), mime::charset_types::utf8.begin(), mime::charset_types::utf8.end());
                filed_iterator+=mime::charset_types::utf8.length() + 3;
                temp = std::search(filed_iterator, subject_field.end(), end_encoded_string.begin(), end_encoded_string.end());
            }while(temp != subject_field.end());


            return this->QPDecode(buffer);
        }
    }

    return subject_field;
}

std::string Message::base64Decode(std::string encoded_string){
    return base64_decode(encoded_string, true);
}

std::string Message::QPDecode(std::string encoded_string){
    std::string decoded_string;
    decoded_string.clear();

    for(std::string::iterator iter = encoded_string.begin(); iter < encoded_string.end(); ++iter){
        if(*iter == '=' && (iter + 2) < encoded_string.end()){
            std::stringstream ss;
            std::string hex_symbol{*(iter+1), *(iter + 2)};
            if(hex_symbol == "\r\n"){
                decoded_string += "\n";
            }
            else{
                ss << std::hex << hex_symbol;
                unsigned int ascii_code = std::stoul(hex_symbol, nullptr, 16);
                decoded_string += ascii_code;
            }
            iter+=2;
        }
        else{
            if(*iter != '=')
                decoded_string += *iter;
        }
    }

    return decoded_string;
}

std::string Message::getSenderContext(){
    return this->sender;
}

std::string Message::getFromFieldContext(){
    return this->from;
}

std::string Message::getRecipientContext(){
    return this->recipient;
}

std::string Message::getSubjectContext(){
    return this->subject;
}

std::string Message::getDataContext(){
    return this->data;
}

std::string Message::getContentTypeContext(){
    return this->content_type;
}

void Message::show(){
    std::cout << "Sender : " << this->sender << std::endl;
    std::cout << "From : " << this->from << std::endl;
    std::cout << "To : " << this->recipient << std::endl;
    std::cout << "Contnet-Type : " << this->content_type << std::endl;
    std::cout << "Subject : " << this->subject << std::endl;
    std::cout << "Context : " << this->data << std::endl;
}