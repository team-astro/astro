/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/dns.h>
#include <chrono>
#include <iterator>

using namespace astro;
using namespace astro::net;

using namespace std;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

TEST dns_resolution_succeeds() {
  using dns_name = std::tuple<const char*, std::vector<std::string>>;
  dns_name dns_names[] = {
    dns_name("localhost", {"127.0.0.1", "::1"}),
    dns_name("a.root-servers.net", {"198.41.0.4", "2001:503:ba3e::2:30"}),
    dns_name("b.root-servers.net", {"192.228.79.201", "2001:500:84::b"}),
    dns_name("c.root-servers.net", {"192.33.4.12", "2001:500:2::c"}),
    dns_name("d.root-servers.net", {"199.7.91.13", "2001:500:2d::d"}),
    dns_name("e.root-servers.net", {"192.203.230.10"}),
    dns_name("f.root-servers.net", {"192.5.5.241", "2001:500:2f::f"}),
    dns_name("g.root-servers.net", {"192.112.36.4"}),
    dns_name("h.root-servers.net", {"128.63.2.53", "2001:500:1::803f:235"}),
    dns_name("i.root-servers.net", {"192.36.148.17", "2001:7fe::53"}),
    dns_name("j.root-servers.net", {"192.58.128.30", "2001:503:c27::2:30"}),
    dns_name("k.root-servers.net", {"193.0.14.129", "2001:7fd::1"}),
    dns_name("l.root-servers.net", {"199.7.83.42", "2001:500:3::42"}),
    dns_name("m.root-servers.net", {"202.12.27.33", "2001:dc3::35"})
  };

  for (auto&& addr : dns_names)
  {
    const char* test_dns;
    std::vector<std::string> test_ips;
    std::vector<std::string> result_ips;
    std::tie(test_dns, test_ips) = addr;

    auto addresses = dns::resolve_host_name(test_dns, strlen(test_dns)).get();
    transform(begin(addresses), end(addresses), std::back_inserter(result_ips),
      [](ip_address& ip)
      {
        char s[INET6_ADDRSTRLEN];
        ip_address_to_string(s, sizeof(s), &ip);
        return std::string(s);
      });

    for (auto& ip : test_ips)
    {
      ASSERT(find(begin(result_ips), end(result_ips), ip) != end(result_ips));
    }
  }

  PASS();
}

SUITE(dns_tests) { RUN_TEST(dns_resolution_succeeds); }
