#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <future>
#include <tuple>
#include <vector>

namespace astro { namespace net
{
  std::future<std::vector<ip_address>>
  dns::resolve_host_name(const char* hostname, uintptr hostname_len, address_family preferred_family)
  {
    return std::async(std::launch::async,
      [](const char* name, uintptr name_len, address_family family)
      {
        uintptr address_count = 0;
        std::vector<ip_address> result;
        struct addrinfo hints = {};
        hints.ai_family = PF_UNSPEC;
        hints.ai_flags = AI_CANONNAME;
        hints.ai_socktype = SOCK_STREAM;
        struct addrinfo* infos = nullptr;

        char host[NI_MAXHOST] = {};
        char srv[NI_MAXSERV] = {};

        strncpy(host, name, min(sizeof(host), name_len));

        getaddrinfo(host, nullptr, &hints, &infos);
        for (struct addrinfo* info = infos; info; info = info->ai_next)
        {
          if (getnameinfo(info->ai_addr, info->ai_addrlen, host, sizeof(host),
            nullptr, 0, NI_NUMERICHOST) == 0)
          {
            ip_address ip = parse_ip_address(host);
            if (family != address_family::none && ip.family != family)
              continue;

            result.push_back(ip);
          }
        }

        freeaddrinfo(infos);
        return result;
      }, hostname, hostname_len, preferred_family);
  }
}}
