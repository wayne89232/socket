// Implementation of the ClientSocket class
#include <iostream>
#include "ClientSocket.h"
#include "SocketException.h"
#include <string>
#include "openssl/ssl.h"
#include "openssl/err.h"

SSL_CTX* InitCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = (SSL_METHOD*)SSLv3_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
    
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    std::cout << ssl << cert << "\n";
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

ClientSocket::ClientSocket ( std::string host, int port )
{
    // this->onPeer = false;
    
    if ( ! Socket::create() )
    {
        throw SocketException ( "Could not create client socket." );
    }
    SSL_library_init();
    this->ctx = InitCTX();
    
    if ( ! Socket::connect ( host, port ) )
    {
        throw SocketException ( "Could not connect." );
    }
    if ( SSL_connect(this->ssl) == -1 )
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(this->ssl);
    }
}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
    if ( ! Socket::send ( s ) )
    {
        throw SocketException ( "Could not write to socket." );
    }

    return *this;

}


const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
    if ( ! Socket::recv ( s ) )
    {
        throw SocketException ( "Could not read from socket." );
    }

    return *this;
}

void ClientSocket::accept ( ClientSocket& sock )
{
    if ( ! Socket::accept ( sock ) )
    {
        throw SocketException ( "Could not accept socket." );
    }
}

SSL_CTX*  ClientSocket::get_ctx()
{
  return this->ctx;
}
SSL* ClientSocket::get_ssl()
{
  return this->ssl;
}


void ClientSocket::close ()
{
    Socket::close();
}


