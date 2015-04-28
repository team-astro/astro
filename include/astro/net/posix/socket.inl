#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

namespace astro { namespace net
{
  struct socket
  {
    int socket;
    struct addrinfo* info;
    bool32 is_bound;
    bool32 is_listening;
    bool32 is_connected;
    ip_address ip;
    uint16 port;

  };

  static char s_hostname[256];

  socket
  socket_create(address_family family, socket_type type, protocol_type protocol)
  {
    if (s_hostname[0] == '\0')
    {
      if (gethostname(s_hostname, sizeof(s_hostname)) < 0)
      {
        log_error("Error retrieving hostname");
      }
      else
      {
        log_debug("Hostname: %s", s_hostname);
      }
    }

    socket result = {};

    struct addrinfo hints = {};
    hints.ai_family = (int)family;
    hints.ai_socktype = (int)type;
    if (getaddrinfo(s_hostname, nullptr, &hints, &result.info) < 0)
    {
      log_error("Error calling getaddrinfo.");
    }
    else
    {
      result.ip.family = family;
      result.socket = ::socket((int)family, (int)type, (int)protocol);
    }

    return result;
  }

  bool32
  socket_bind(socket* s, ip_address ip, uint16 port)
  {
    bool32 result = true;
    if (s && s->socket && s->ip.family == ip.family)
    {
      int yes = 1;
      if (::setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        log_debug("Error setting SO_REUSEADDR on socket. Bind may fail.");

      if (::bind(s->socket, s->info->ai_addr, s->info->ai_addrlen) < 0)
      {
        log_error("Error binding socket to port %d", port);
        result = false;
      }

    //   switch(ip.family)
    //   {
    //   case address_family::inter_network:
    //     {
    //       struct sockaddr_in sa = {};
    //       sa.sin_family = (sa_family_t)ip.family;
    //       sa.sin_addr.s_addr = htonl(ip_address_as_v4_int(&ip));
    //       sa.sin_port = htons(port);
    //       memset(sa.sin_zero, '\0', sizeof(sa.sin_zero));
    //
    //       log_debug("sin_family %d", sa.sin_family);
    //       log_debug("AF_INET: %d", AF_INET);
    //       log_debug("sin_addr: %d", sa.sin_addr.s_addr);
    //       log_debug("sin_port: %d", sa.sin_port);
    //
    //       if (::bind(s->socket, (sockaddr*)&sa, sizeof(sockaddr_in)) < 0)
    //       {
    //         log_error("Error binding socket to port %d", port);
    //         result = false;
    //       }
    //     }
    //   case address_family::inter_network_v6:
    //     {
    //       struct sockaddr_in6 sa = {};
    //       sa.sin6_family = (sa_family_t)ip.family;
    //       sa.sin6_port = htons(port);
    //       sa.sin6_scope_id = ip.scope;
    //
    //       memcpy(sa.sin6_addr.s6_addr, ip.addr, sizeof(ip.addr));
    //
    //       if (::bind(s->socket, (sockaddr*)&sa, sizeof(sockaddr_in6)) < 0)
    //       {
    //         log_error("Error binding socket to port %d", port);
    //         result = false;
    //       }
    //     }
    //     break;
    //   default:
    //     // TODO: Support unix sockets at a minimum.
    //     result = false;
    //     break;
    //   }
    }

    if (result)
    {
      s->is_bound = true;
      s->ip = ip;
      s->port = port;
    }

    return result;
  }

  bool32
  socket_listen(socket* s, int32 backlog)
  {
    if (!s) return false;

    if (!s->is_bound)
    {
      log_error("Attempting to listen on unbound socket.");
      return false;
    }

    if (::listen(s->socket, backlog) < 0)
    {
      log_error("Error listening on socket.");
      return false;
    }

    s->is_listening = true;
    return true;
  }

  bool32
  socket_connect(socket* s, ip_address ip, uint16 port)
  {
    if (!s) return false;

    bool32 result = true;
    char port_str[10];
    sprintf(port_str, "%d", port);

    struct addrinfo* res;
    struct addrinfo hints = {};
    hints.ai_family = s->info->ai_family;
    hints.ai_socktype = s->info->ai_socktype;
    if (getaddrinfo(ip_address_to_string(&ip), port_str, &hints, &res) < 0)
    {
      log_error("Error calling getaddrinfo.");
    }
    else
    {
      // result.ip.family = family;
      // result.socket = ::socket((int)family, (int)type, (int)protocol);

      if (::connect(s->socket, res->ai_addr, res->ai_addrlen) < 0)
      {
        log_error("Error connecting socket to %s:%d", ip_address_to_string(&ip), port);
        result = false;
      }
    }


    // switch(ip.family)
    // {
    // case address_family::inter_network:
    //   {
    //     struct sockaddr_in sa = {};
    //     sa.sin_family = (sa_family_t)ip.family;
    //     sa.sin_addr.s_addr = htonl(ip_address_as_v4_int(&ip));
    //     sa.sin_port = htons(port);
    //
    //     if (::connect(s->socket, (sockaddr*)&sa, sizeof(sockaddr_in)) < 0)
    //     {
    //       log_error("Error connecting socket to %s:%d", ip_address_to_string(&ip), port);
    //       result = false;
    //     }
    //   }
    // case address_family::inter_network_v6:
    //   {
    //     struct sockaddr_in6 sa = {};
    //     sa.sin6_family = (sa_family_t)ip.family;
    //     sa.sin6_port = htons(port);
    //     sa.sin6_scope_id = ip.scope;
    //
    //     memcpy(sa.sin6_addr.s6_addr, ip.addr, sizeof(ip.addr));
    //
    //     if (::connect(s->socket, (sockaddr*)&sa, sizeof(sockaddr_in6)) < 0)
    //     {
    //       log_error("Error connecting socket to %s:%d", ip_address_to_string(&ip), port);
    //       result = false;
    //     }
    //   }
    //   break;
    // default:
    //   // TODO: Support unix sockets at a minimum.
    //   result = false;
    //   break;
    // }

    if (result)
    {
      s->ip = ip;
      s->port = port;
    }

    return result;
  }

  socket
  socket_accept(socket* s)
  {
    socket result = {};
    if (s && s->socket && s->is_listening)
    {
      struct sockaddr_storage ss = {};
      socklen_t ss_len = sizeof(sockaddr_storage);
      int conn = ::accept(s->socket, (sockaddr*)&ss, &ss_len);
      if (conn < 0)
      {
        log_error("Error accepting connection on socket.");
        return result;
      }

      result.socket = conn;
      address_family family = (address_family) ss.ss_family;
      switch (family)
      {
      case address_family::inter_network:
        {
          struct sockaddr_in* sa = (sockaddr_in*)&ss;
          result.ip = ip_address(ntohl(sa->sin_addr.s_addr));
          result.port = ntohs(sa->sin_port);
        }
      case address_family::inter_network_v6:
        {
          struct sockaddr_in6* sa = (sockaddr_in6*)&ss;
          result.port = ntohs(sa->sin6_port);
          result.ip.family = address_family::inter_network_v6;
          result.ip.scope = sa->sin6_scope_id;
          memcpy(result.ip.addr, sa->sin6_addr.s6_addr, sizeof(result.ip.addr));
        }
        break;
      default:
        // TODO: Support unix sockets at a minimum.
        break;
      }
    }

    return result;
  }

  void
  socket_close(socket* s)
  {
    if (s && s->socket)
    {
      ::close(s->socket);
      *s = {};
    }
  }

  bool32
  socket_send(socket* s, uint8* data, uintptr data_len)
  {
    if (s && s->socket && s->is_connected)
    {
      do
      {
        int res = ::send(s->socket, (void*)data, data_len, 0);
        if (res < 0)
        {
          log_error("Error sending data over socket.");
          break;
        }

        data_len -= res;
        data += res;
      } while (data_len > 0);
    }

    return data_len == 0;
  }

  bool32
  socket_recv(socket* s, uint8* data, uintptr data_len)
  {
    if (s && s->socket && s->is_connected)
    {
      do
      {
        int res = ::recv(s->socket, (void*)data, data_len, 0);
        if (res < 0)
        {
          log_error("Error recieving data from socket.");
          break;
        }

        if (res == 0)
        {
          // TODO: Send notification of socket closing?
          socket_close(s);
          break;
        }

        data_len -= res;
        data += res;
      } while (data_len > 0);
    }

    return data_len == 0;
  }
}}
