#include <iostream>
#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
#include <list>
#include "openssl/ssl.h"
#include "openssl/err.h"
// #include <thread>

using namespace std;

void* res(void* new_sock);

void ShowCerts(SSL* ssl);


list<Clients> user_list;

int main ()
{
  	std::cout << "Running....\n";


 	try
    {
      	// Create the socket
      	ServerSocket server ( 30000 );
    	int count = 0;
		pthread_t respond[10000];
		ServerSocket new_sock[10000];
    	while ( true )
		{	
			
	  		server.accept ( new_sock[count] );
	  		
	  		if(SSL_accept(new_sock[count].get_ssl()) == -1){
	  			ERR_print_errors_fp(stderr);
	  		}
	  		else
	  		{
		  			ShowCerts(new_sock[count].get_ssl());	
		  		
		  		std::cout << "Connection established.\n";
					  		
		  		pthread_create( &respond[count], NULL, res, &(new_sock[count]) );
				count++;
			}
		}
    }
  	catch ( SocketException& e )
    {
      	std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

  	return 0;
}

void* res(void* new_sock)
{
	ServerSocket new_sock2 = *((ServerSocket*)new_sock);
	// cout << new_sock2.get_m_sock() << "\n";
	try
	{
		
		while( true )
		{
			std::string data;
			while( data.length() == 0 )
			{
				new_sock2 >> data;
			}
			new_sock2 << new_sock2.respond( data, user_list );
			// std::cout << data <<"\n";
		}
	}
	catch ( SocketException& e ) {
		std::cout << "Connection broken." << "\nExiting.\n";
	}
}
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    // cout << ssl << " " << cert << "\n";
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

