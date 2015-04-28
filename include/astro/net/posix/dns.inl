#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <future>
#include <tuple>

namespace astro { namespace net
{
  std::future<ip_address>
  dns::resolve_host_name(const char* hostname)
  {
    return std::async(std::launch::async, [](const char* name)
    {
      uintptr address_count = 0;
      ip_address result;
      struct addrinfo hints = {};
      hints.ai_family = PF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      struct addrinfo* infos = nullptr;
      getaddrinfo(name, nullptr, &hints, &infos);

      char host[NI_MAXHOST];
      char srv[NI_MAXSERV];

      int s = -1;
      for (struct addrinfo* info = infos; info; info = info->ai_next)
      {
        if (getnameinfo(info->ai_addr, info->ai_addrlen, host, sizeof(host),
          nullptr, 0, NI_NUMERICHOST) == 0)
        {
          result = parse_ip_address(host);
          if (result.family != ip_family::none)
            break;
        }
      }

      freeaddrinfo(infos);
      return result;
    }, hostname);
  }
}}
