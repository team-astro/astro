/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/dns.h>

using namespace astro;
using namespace astro::net;

TEST dns_resolution_succeeds() {
  std::tuple<const char*, const char*, const char*> dns_names[] = {
    std::make_tuple("localhost", "127.0.0.1", "::1"),
    std::make_tuple("a.root-servers.net", "198.41.0.4", "2001:503:ba3e::2:30"),
    std::make_tuple("b.root-servers.net", "192.228.79.201", "2001:500:84::b"),
    std::make_tuple("c.root-servers.net", "192.33.4.12", "2001:500:2::c"),
    std::make_tuple("d.root-servers.net", "199.7.91.13", "2001:500:2d::d"),
    std::make_tuple("e.root-servers.net", "192.203.230.10", nullptr),
    std::make_tuple("f.root-servers.net", "192.5.5.241", "2001:500:2f::f"),
    std::make_tuple("g.root-servers.net", "192.112.36.4", nullptr),
    std::make_tuple("h.root-servers.net", "128.63.2.53", "2001:500:1::803f:235"),
    std::make_tuple("i.root-servers.net", "192.36.148.17", "2001:7fe::53"),
    std::make_tuple("j.root-servers.net", "192.58.128.30", "2001:503:c27::2:30"),
    std::make_tuple("k.root-servers.net", "193.0.14.129", "2001:7fd::1"),
    std::make_tuple("l.root-servers.net", "199.7.83.42", "2001:500:3::42"),
    std::make_tuple("m.root-servers.net", "202.12.27.33", "2001:dc3::35")
  };

  for (auto&& addr : dns_names)
  {
    const char* test_dns;
    const char* test_ipv4;
    const char* test_ipv6;
    std::tie(test_dns, test_ipv4, test_ipv6) = addr;

    ip_address address;
    address = dns::resolve_host_name(test_dns).get();

    ASSERT_EQ(ip_family::inter_network, address.family);
  }

  PASS();
}

SUITE(dns_tests) { RUN_TEST(dns_resolution_succeeds); }
