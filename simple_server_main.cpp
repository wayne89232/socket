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



list<Clients> user_list;

int main ( int argc, char* argv[] )
{
  	std::cout << "Running....\n";


 	try
    {
      	// Create the socket
      	ServerSocket server ( 30000 );
    	int count = 0;
		pthread_t respond[100000];
		ServerSocket new_sock[100000];
    	while ( true )
		{
	  		server.accept ( new_sock[count] );
	  		std::cout << "Connection established.\n";

	  		// cout << new_sock[count].get_m_sock() << "\n";
				  		
	  		pthread_create( &respond[count], NULL, res, &(new_sock[count]) );
			count++;
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
