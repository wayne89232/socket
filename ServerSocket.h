// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"
#include <string>
#include <list>
#include "openssl/ssl.h"
#include "openssl/err.h"
using namespace std;

class Clients
{
	public:
		//store name ip port
		string name;
		string ip;
		string port;
		int sockfd;
		Clients(string name, string ip, string port, int sockfd)
		{
			this->name = name;
			this->ip = ip;
			this->port = port;
			this->sockfd = sockfd;
		}

};

class ServerSocket : private Socket
{
	public:
	 	ServerSocket ( int port );
	  	ServerSocket (){};
		virtual ~ServerSocket();

		const ServerSocket& operator << ( const std::string& ) const;
	    const ServerSocket& operator >> ( std::string& ) const;

  		void accept ( ServerSocket& );
    	std::string respond ( std::string, list<Clients>& user_list );
    	int get_m_sock();
    	SSL_CTX*  get_ctx();
    	SSL* get_ssl();
};


#endif
