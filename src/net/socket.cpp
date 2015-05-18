#include <astro/astro.h>

#if ASTRO_PLATFORM_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")
#define close closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <astro/net/socket.h>

namespace astro { namespace net
{
  static char s_hostname[256];

#if ASTRO_PLATFORM_WIN32
  static bool winsock_started = false;

  bool init_socket()
  {
    if (!winsock_started)
    {
      WORD version_requested = MAKEWORD(2, 2);
      WSADATA wsa = {};
      int err = WSAStartup(version_requested, &wsa);
      if (err != 0)
      {
        log_error("Error initializing sockets.");
        return false;
      }

      if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2)
      {
        log_error("No usable version of winsock.");
        return false;
      }

      winsock_started = true;
    }

    return true;
  }
#else
  bool init_socket() { return true; }
#endif

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
      ((struct sockaddr_in*)sa)->sin_port = htons(port);
    }
    else
    {
      ((struct sockaddr_in6*)sa)->sin6_port = htons(port);
    }
  }

  // struct sockaddr_result
  // {
  //   struct sockaddr_storage sa;
  //   uint16 sa_len;
  // };
  //
  // sockaddr_result
  // ip_address_to_sockaddr(ip_address ip, uint16 port)
  // {
  //   sockaddr_result result = {};
  //   result.sa
  // }

  void
  socket_destroy(socket* sock)
  {
    if (sock)
    {
      ::close(sock->s);
      *sock = {};
    }
  }

  socket
  socket_create(address_family family, socket_type type, protocol_type protocol)
  {
    socket result = {};
    if (!init_socket())
      return result;

    if (s_hostname[0] == '\0')
    {
      if (gethostname(s_hostname, sizeof(s_hostname)) < 0)
      {
        log_error("Error retrieving hostname");
        return result;
      }
    }

    result.family = family;
    result.type = type;
    result.s = ::socket((int)family, (int)type, (int)protocol);

    return result;
  }

  bool32
  socket_bind(socket* s, ip_address ip, uint16 port)
  {
    bool32 result = true;
    if (s && s->s && s->family == ip.family)
    {
      char yes = 1;
      if (::setsockopt(s->s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        log_warn("Error setting SO_REUSEADDR on socket. Bind may fail.");

      struct addrinfo* res;
      struct addrinfo hints = {};
      hints.ai_family = (int) s->family;
      hints.ai_socktype = (int) s->type;
      char ip_str[256];
      char port_str[10];
      sprintf(port_str, "%d", port);

      if (ip == ip_address::any)
      {
        hints.ai_flags = AI_PASSIVE;
        ip_address_to_string(ip_str, sizeof(ip_str), &ip);
      }

      int gai_res = getaddrinfo(ip_str, port_str, &hints, &res);
      if (gai_res != 0)
      {
        log_error("Error in getaddrinfo: %s", gai_strerror(gai_res));
        result = false;
      }
      else
      {
        if (::bind(s->s, res->ai_addr, res->ai_addrlen) < 0)
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

    if (::listen(s->s, backlog) < 0)
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
    hints.ai_family = (int) s->family;
    hints.ai_socktype = (int) s->type;

    ip_address_to_string(url, sizeof(url), &ip);
    int gai_res = getaddrinfo(url, port_str, &hints, &res);
    if (gai_res != 0)
    {
      log_error("Error in getaddrinfo: %s", gai_strerror(gai_res));
      result = false;
    }
    else
    {
      if (::connect(s->s, res->ai_addr, res->ai_addrlen) < 0)
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
      s->is_connected = true;
    }

    return result;
  }

  socket
  socket_accept(socket* s)
  {
    socket result = {};
    if (s && s->s && s->is_listening)
    {
      struct sockaddr_storage ss = {};
      socklen_t ss_len = sizeof(sockaddr_storage);
      int conn = ::accept(s->s, (sockaddr*)&ss, &ss_len);
      if (conn < 0)
      {
        log_error("Error accepting connection on socket.");
        return result;
      }

      result.s = conn;
      result.is_connected = true;
      result.family = (address_family) ss.ss_family;
      switch (result.family)
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
    if (s && s->s)
    {
      ::close(s->s);
      *s = {};
    }
  }

  bool32
  socket_send(socket* s, uint8* data, uintptr data_len)
  {
    if (s && s->s && s->is_connected)
    {
      do
      {
        int res = ::send(s->s, (char*)data, data_len, 0);
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

  int32
  socket_recv(socket* s, uint8* data, uintptr data_len)
  {
    if (s && s->s && s->is_connected)
    {
      int res = ::recv(s->s, (char*)data, data_len, 0);
      if (res < 0)
      {
        log_error("Error recieving data from socket.");
      }
      else if (res == 0)
      {
        // TODO: Send notification of socket closing?
        socket_close(s);
      }

      return res;
    }

    return 0;
  }
}}
