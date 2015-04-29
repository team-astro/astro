#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace astro { namespace net
{
  struct socket
  {
    int socket;
    socket_type type;
    bool32 is_bound;
    bool32 is_listening;
    bool32 is_connected;
    ip_address ip;
    uint16 port;

  };

  static char s_hostname[256];

  void *get_in_addr(struct sockaddr *sa)
  {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }

  void set_addr_port(struct sockaddr *sa, uint16 port)
  {
    if (sa->sa_family == AF_INET)
    {
      ((struct sockaddr_in*)sa)->sin_port = htonl(port);
    }
    else
    {
      ((struct sockaddr_in6*)sa)->sin6_port = htonl(port);
    }
  }

  socket
  socket_create(address_family family, socket_type type, protocol_type protocol)
  {
    if (s_hostname[0] == '\0')
    {
      if (gethostname(s_hostname, sizeof(s_hostname)) < 0)
      {
        log_error("Error retrieving hostname");
      }
    }

    socket result = {};
    result.ip.family = family;
    result.type = type;
    result.socket = ::socket((int)family, (int)type, (int)protocol);

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
        log_warn("Error setting SO_REUSEADDR on socket. Bind may fail.");

      struct addrinfo* res;
      struct addrinfo hints = {};
      hints.ai_family = (int) s->ip.family;
      hints.ai_socktype = (int) s->type;
      char ip_str[256];
      char port_str[10];
      sprintf(port_str, "%d", port);

      if (ip == ip_address::any)
      {
        hints.ai_flags = AI_PASSIVE;
        ip_address_to_string(ip_str, sizeof(ip_str), &ip);
      }

      if (getaddrinfo(ip_str, port_str, &hints, &res) < 0)
      {
        log_error("Error calling getaddrinfo.");
        result = false;
      }
      else
      {
        if (::bind(s->socket, res->ai_addr, res->ai_addrlen) < 0)
        {
          log_error("Error binding socket to port %d", port);
          result = false;
        }
      }
      
      freeaddrinfo(res);
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
    char url[256];
    sprintf(port_str, "%d", port);

    struct addrinfo* res;
    struct addrinfo hints = {};
    hints.ai_family = (int) s->ip.family;
    hints.ai_socktype = (int) s->type;

    ip_address_to_string(url, sizeof(url), &ip);
    if (getaddrinfo(url, port_str, &hints, &res) < 0)
    {
      log_error("Error calling getaddrinfo.");
    }
    else
    {
      if (::connect(s->socket, res->ai_addr, res->ai_addrlen) < 0)
      {
        log_error("Error connecting socket to %s:%d", url, port);
        result = false;
      }
    }
    
    freeaddrinfo(res);

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
