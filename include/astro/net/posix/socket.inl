#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace astro { namespace net
{
  astro::net::socket
  create(uint16 port)
  {
    socket result = {};
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    if (sock >= 0)
    {
      struct sockaddr_in name = {};
      name.sin_family = AF_INET;
      name.sin_port = htons(port);
      name.sin_addr.s_addr = htonl(INADDR_ANY);


    }

    return result;
  }
}}
