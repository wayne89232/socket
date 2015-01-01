#include "ClientSocket.h"
#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
// #include <string.h>
#include <pthread.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

using namespace std;

void* peering(void* port);
void ShowCerts(SSL* ssl);
int a;



int main  ( int argc, char* argv[] ) 
{
    string menu ( "Select what to do:\n"); 
    menu += "1. Register to server\n";
    menu += "2. Login \n";
    menu += "3. List users online\n";
    menu += "4. Talk to someone\n";
    menu += "5. Exit\n";
    try
    {
        ClientSocket client_socket ( "127.0.0.1", 30000 ) ;  //"140.112.107.39", 5900 
        
        pthread_mutex_t mutex;       
        while ( true ) 
        {
           // cout <<  SSL_get_cipher(client_socket.get_ssl()) << "\n";
            // pthread_mutex_lock (&mutex);
            pthread_t waitcon;
            usleep(100);
            cout << menu;
            int act;
            cin >> act;
            string res;
            string chat_port;
            if ( act == 1 )
            {
                cout << "Please input username: \n";
                string name;
                cin >> name;
                cout << "Please input port: \n";
                cin >> chat_port;cout << " \n";
                string port = "#"+chat_port;
                client_socket << "REGISTER#" + name + port; 
                client_socket >> res;
                cout << "Server: " << res << "\n";
            }
            else if ( act == 2 )
            {
                cout << "Please input username: \n";
                string name2;
                cin >> name2;
                client_socket << "List";
                client_socket >> res;
                cout << res << "\n";
                //parse port to bind
                size_t pos = res.find("\n"+name2+"#");
                pos = res.find("#", pos);
                pos = res.find("#", pos+1);
                size_t tail = res.find('\r', pos+1);
                a = pos+1;
                string port;
                while( a < tail ){
                    port += res[a];
                    a++;
                } 
                //bind to port
                int x = atoi( port.c_str() );
                int* port2 = &x;
                pthread_create(&waitcon, NULL, peering, static_cast<void*>( port2 ));        
                std::cout << "Binded to port " << port << "\n";
                a = 0;
            }
            else if ( act == 3 )
            {
                client_socket << "List";
                client_socket >> res;
                cout << res << "\n";
                //find someone
                // cout << " ";
                size_t pos = res.find("#");
                // cout << res[pos+1] << "\n";
            }
            else if ( act == 4)
            {
                string peer;
                cout << "Who do u want to talk: \n";
                cin >> peer;
                client_socket << "List";
                client_socket >> res;
                size_t pos = res.find("\n"+peer+"#");
                size_t pos1 = res.find("#", pos) + 1;
                size_t pos2 = res.find("#", pos1) - 1;
                size_t pos3 = res.find("#", pos1) + 1;
                size_t tail = res.find('\r', pos3) - 1;cout << " ";
                a = pos1;
                string p_ip;
                while( a <= pos2 ){
                    p_ip += res[a];
                    a++;
                }
                a = pos3;       
                string p_port;
                while( a <= tail ){
                    p_port += res[a];
                    a++;
                }
                cout << p_ip << p_port << "\n";          
                // string ip;
                // cout << "Please input ip to talk to: ";
                // cin >> ip;                
                // int port2;
                // cout << "Please input port to talk to: ";
                // cin >> port2;
                ClientSocket client_socket2 ( p_ip, atoi( p_port.c_str() ) + 1 );
                cout << "Connect to peer!\n";
                client_socket2.onPeer = true;
                while( client_socket2.onPeer == true )
                {
                    string reply;
                    usleep( 100 );
                    cout << "Talk: \n";
                    cin >> reply; 
                    if( reply == "bye" )
                    {
                        client_socket2 << "bye~";
                        client_socket2.close();
                        client_socket2.onPeer = false;
                        break;
                    }
                    client_socket2 << reply;
                }
                client_socket2.onPeer = false;
            }
            else if ( act == 5 )
            {
                string res;
                client_socket << "Exit";
                client_socket >> res;
                cout << res << "\n";
                client_socket.close();   
                break;
            }
            else
            {
                cout << "fail";
                string a;          
                // client_socket << "List";
                // client_socket >> res;
                // int num = res[1];
                // int name = res.find("\n");

                // cout << num << " " << name << "\n";
            }

            // pthread_mutex_unlock (&mutex);
            
        }
    }
    catch ( SocketException& e ) 
    {
        cout << "Exception was caught:" << e.description ()  << "\n";
    }
    return 0;
}

void* peering(void* port) //for listenning to other peer : void* client_socket
{
    int* port1 = (int*)port;
    // ClientSocket* client_socket1 = static_cast<ClientSocket*>(client_socket);
    //start listenning    
    try
    {
        // Create the socket
        ServerSocket server ( (*port1) + 1 );

        while ( true )
        {

            ServerSocket new_sock;
            server.accept ( new_sock );
            if(SSL_accept(new_sock.get_ssl()) == -1){
                ERR_print_errors_fp(stderr);
            }
            else{
                ShowCerts(new_sock.get_ssl());   

                std::cout << "Connection established.\n";
                try
                {
                    while ( true )
                    {
                        
                        std::string data;
                        new_sock >> data;
                        // new_sock << new_sock.respond (data);
                        cout << "Peer: " << data << "\n";
                    }
                    
                }
                catch ( SocketException& e) {
                    cout << "Connection broken.\n";
                }
            }
        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }
}

// void ShowCerts(SSL* ssl)
// {   X509 *cert;
//     char *line;
 
//     cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
//     // cout << ssl << " " << cert << "\n";
//     if ( cert != NULL )
//     {
//         printf("Server certificates:\n");
//         line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
//         printf("Subject: %s\n", line);
//         free(line);
//         line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
//         printf("Issuer: %s\n", line);
//         free(line);
//         X509_free(cert);
//     }
//     else
//         printf("No certificates.\n");
// }

