// Implementation of the ServerSocket class
#include <iostream>
#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
#include <arpa/inet.h>
#include <list>
#include <stdlib.h>
#include <sstream>
#include "openssl/ssl.h"
#include "openssl/err.h"


using namespace std;

SSL_CTX* InitServerCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = SSLv3_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

ServerSocket::ServerSocket ( int port )
{

  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }
  Socket::set_non_blocking ( false );
  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }
  SSL_library_init();
  this->ctx = InitServerCTX();
  LoadCertificates(this->ctx, "mycert.pem", "mycert.pem");
  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }
}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }
    // std::cout << m_sock << "\n";
}

int ServerSocket::get_m_sock()
{
  // std::cout << inet_ntoa(this->m_addr.sin_addr) << "\n"; 
  return this->m_sock; 
 
}
SSL_CTX*  ServerSocket::get_ctx()
{
  return this->ctx;
}
SSL* ServerSocket::get_ssl()
{
  return this->ssl;
}

std::string ServerSocket::respond (std::string recv, list<Clients>& user_list)
{
    //if not exist in list, only allow exit and register
    std::size_t pos = recv.find( '#' );
    std::size_t pos1 = recv.find( '#', pos + 1 );
    if( recv == "Exit" ){
        for( list<Clients>::iterator i = user_list.begin(); i != user_list.end(); i++ )
        {
            if( (*i).sockfd == this->m_sock );
                i = user_list.erase(i);
        }
        return "Bye~\n";
    }
    else if ( recv.substr(0, pos ) == "REGISTER" )
    {
        for( list<Clients>::iterator i = user_list.begin(); i != user_list.end(); i++ )
        {
            if( (*i).sockfd == this->m_sock )
                return "Already registerd!\n";
            else if( (*i).port == recv.substr( pos1+1 ) )
                return "Port already in use!\n";
        }
        Clients new_client ( recv.substr( pos+1, pos1-(pos+1) ), "127.0.0.1", recv.substr( pos1+1 ), this->m_sock );
        user_list.push_back( new_client );
        cout << "\nUser List: \n";
        for( list<Clients>::iterator i=user_list.begin(); i!=user_list.end(); i++ )
        {
                cout << (*i).name << "#" << (*i).ip << "#" << (*i).port << "\r\n";
        }
        return "100 OK\n";
    }
    else if ( recv == "List" )
    {
        stringstream ss;

        ss << user_list.size();
        string u_list = ss.str() + "\r\n";
        for( list<Clients>::iterator i=user_list.begin(); i!=user_list.end(); i++ )
        {
            string temp = (*i).name + "#" + (*i).ip + "#" + (*i).port + "\r\n";
            u_list += temp;
        }
        return u_list;
    }
    else{
        return "Unexpected command.\n";
    }
}

