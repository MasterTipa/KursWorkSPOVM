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

#include <array> // replace standart arrays to std::array
#include <algorithm>

#include "tcp_socket.hpp"
#include "ssl_socket.hpp"

SocketSSL::SocketSSL(std::string dest_adr, int port){
     /* ---------------------------------------------------------- *
     * These function calls initialize openssl for correct work.  *
     * ---------------------------------------------------------- */
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    /* ---------------------------------------------------------- *
     * Create the Input/Output BIO's.                             *
     * ---------------------------------------------------------- */
    this->certbio = BIO_new(BIO_s_file());
    this->outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

    /* ---------------------------------------------------------- *
     * initialize SSL library and register algorithms             *
     * ---------------------------------------------------------- */
    if(SSL_library_init() < 0)
      BIO_printf(this->outbio, "Could not initialize the OpenSSL library !\n");

    /* ---------------------------------------------------------- *
     * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
     * ---------------------------------------------------------- */
    this->method = SSLv23_client_method();

    /* ---------------------------------------------------------- *
     * Try to create a new SSL context                            *
     * ---------------------------------------------------------- */
    if ( (this->ctx = SSL_CTX_new(this->method)) == NULL)
      BIO_printf(this->outbio, "Unable to create a new SSL context structure.\n");

    /* ---------------------------------------------------------- *
     * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
     * ---------------------------------------------------------- */
    SSL_CTX_set_options(this->ctx, SSL_OP_NO_SSLv2);

    /* ---------------------------------------------------------- *
     * Create new SSL connection state object                     *
     * ---------------------------------------------------------- */
    this->ssl = SSL_new(this->ctx);

    /* ---------------------------------------------------------- *
     * Make the underlying TCP socket connection                  *
     * ---------------------------------------------------------- */
    this->socket_tcp = new SocketTCP(dest_adr, port);
    if(this->socket_tcp->getSocketFd() != 0)
      BIO_printf(this->outbio, "Successfully made the TCP connection to: %s.\n", dest_adr.c_str());

    /* ---------------------------------------------------------- *
     * Attach the SSL session to the socket descriptor            *
     * ---------------------------------------------------------- */
    SSL_set_fd(this->ssl, this->socket_tcp->getSocketFd());
    SSL_set_mode(this->ssl, SSL_MODE_AUTO_RETRY);

    /* ---------------------------------------------------------- *
     * Try to SSL-connect here, returns 1 for success             *
     * ---------------------------------------------------------- */
    if ( SSL_connect(this->ssl) != 1 )
      BIO_printf(this->outbio, "Error: Could not build a SSL session to: %s.\n", dest_adr.c_str());
    else
      BIO_printf(this->outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_adr.c_str());

    /* ---------------------------------------------------------- *
     * Get the remote certificate into the X509 structure         *
     * ---------------------------------------------------------- */
    this->cert = SSL_get_peer_certificate(this->ssl);
    if (cert == NULL)
      BIO_printf(this->outbio, "Error: Could not get a certificate from: %s.\n", dest_adr.c_str());
    else
      BIO_printf(this->outbio, "Retrieved the server's certificate from: %s.\n", dest_adr.c_str());

    /* ---------------------------------------------------------- *
     * extract various certificate information                    *
     * -----------------------------------------------------------*/
    this->certname = X509_NAME_new();
    this->certname = X509_get_subject_name(this->cert);

    /* ---------------------------------------------------------- *
     * display the cert subject here                              *
     * -----------------------------------------------------------*/
    BIO_printf(this->outbio, "Displaying the certificate subject data:\n");
    X509_NAME_print_ex(this->outbio, this->certname, 0, 0);
    BIO_printf(this->outbio, "\n");
}

SocketSSL::~SocketSSL(){
     /* ----------------------------------------------------------*
    * Free the structures we don't need anymore                  *
    * -----------------------------------------------------------*/
    SSL_free(this->ssl);
    delete socket_tcp;
    X509_free(this->cert);
    SSL_CTX_free(this->ctx);
    BIO_printf(this->outbio, "Finished SSL/TLS connection with server: %s.\n", this->socket_tcp->getHostName().c_str());
}

int SocketSSL::sendPackage(std::string package){
    int len = SSL_write(this->ssl, (package.c_str()), package.length());
    if (len < 0) {
        int err = SSL_get_error(this->ssl, len);
        switch (err) {
        case SSL_ERROR_WANT_WRITE:
            throw std::string("SSL_ERROR_WANT_WRITE");
        case SSL_ERROR_WANT_READ:
            throw std::string("SSL_ERROR_WANT_READ");
        case SSL_ERROR_ZERO_RETURN:
            throw std::string("SSL_ERROR_ZERO_RETURN");
        case SSL_ERROR_SYSCALL:
            throw std::string("SSL_ERROR_SYSCALL");
        case SSL_ERROR_SSL:
            throw std::string("SSL_ERROR_SSL");
        default:
            return len;
        }
    }
    return 0;
}

std::string SocketSSL::receivePackage(){
    std::array<char, 1024> buf_recv;
    buf_recv.fill(0);
    std::string buff;
    buff.clear();
    int len = 0;
    
    len=SSL_read(this->ssl, buf_recv.data(), sizeof(buf_recv));
    std::for_each(buf_recv.begin(), (buf_recv.begin()+len), [&buff](auto symbol){
      buff+=symbol;
    });
    //BIO_printf(this->outbio, "server: %s" , buff.c_str());
    if (len < 0) {
        int err = SSL_get_error(this->ssl, len);
    if (err == SSL_ERROR_WANT_READ)
            throw std::string("SSL_ERROR_WANT_READ");
        if (err == SSL_ERROR_WANT_WRITE)
            throw std::string("SSL_ERROR_WANT_WRITE");
        if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
            throw std::string("SSL_ERROR_ZERO_RETURN || SSL_ERROR_SYSCALL || SSL_ERROR_SSL");
    }

    return buff;
}