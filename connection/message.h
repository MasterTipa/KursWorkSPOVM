#ifndef EMAIL_MESSAGE
#define EMAIL_MESSAGE

#include <iostream>
#include <string>
#include <map>

namespace mime{
    namespace type_constants{
        const std::string mime_related = "multipart/related";
        const std::string mime_alternative = "multipart/alternative";
        const std::string mime_mixed = "multipart/mixed";
        const std::string mime_html = "text/html";
        const std::string mime_plain = "text/plain";
        const std::string mime_jpeg = "image/jpeg";
        const std::string mime_png = "image/png";
    }

    namespace encoding_type_constants{
        const std::string mime_base64 = "base64";
        const std::string mime_quoted_printable = "quoted-printable";
    }

    namespace charset_types{
        const  std::string utf8 = "utf-8";
    }
}

/*-----------------------------------------------------------*
* Class Message doesn't support parsing attachments in email *
*------------------------------------------------------------*/

class Message{
private:
    std::string sender;
    std::string from;
    std::string recipient;
    std::string subject;
    std::string content_type;
    std::string data;
    std::string boundary;

public:
    Message(std::string sender = "", std::string recipient = "", std::string subject = "",
        std::string content_type = "", std::string data = "") : sender(sender),  recipient(recipient),
        subject(subject), content_type(content_type), data(data), boundary("\r\n"){};

    ~Message(){};

    bool Parse(std::string &);
    void show();
    std::string getSenderContext();
    std::string getFromFieldContext();
    std::string getRecipientContext();
    std::string getSubjectContext();
    std::string getDataContext();
    std::string getContentTypeContext();
private:
    std::string QPDecode(std::string);
    std::string base64Decode(std::string);
    std::vector<std::string> getContentType(std::string &);
    std::string getContentMultipart(std::string &);
    std::string getContentSinglepart(std::string &);
    std::string getEncodingType(std::string &);
    std::string getData(std::string &);
    std::string getSender(std::string &);
    std::string getFromFiled(std::string &);
    std::string getRecipient(std::string &);
    std::string getSubject(std::string &);
    std::string getEncodedBlock(std::string &);
};

#endif // ! - EMAIL_MESSAGE