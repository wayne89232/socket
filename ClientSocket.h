// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.h"
#include "openssl/ssl.h"
#include "openssl/err.h"


class ClientSocket : public Socket
{
 public:

  ClientSocket ( std::string host, int port );
  ClientSocket (int port);
  virtual ~ClientSocket(){};

  const ClientSocket& operator << ( const std::string& ) const;
  const ClientSocket& operator >> ( std::string& ) const;
  void close ();
  void accept ( ClientSocket& );
  bool onPeer;
  SSL_CTX*  get_ctx();
  SSL* get_ssl();
};


#endif
