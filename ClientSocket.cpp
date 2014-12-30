// Implementation of the ClientSocket class
#include <iostream>
#include "ClientSocket.h"
#include "SocketException.h"
#include <string>
#include "openssl/ssl.h"
#include "openssl/err.h"

ClientSocket::ClientSocket ( std::string host, int port )
{
    // this->onPeer = false;
    if ( ! Socket::create() )
    {
        throw SocketException ( "Could not create client socket." );
    }
    if ( ! Socket::connect ( host, port ) )
    {
        throw SocketException ( "Could not connect." );
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
