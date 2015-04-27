/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/ip_address.h>

using namespace astro;
using namespace astro::net;

TEST ipv4_addresses_parse_correctly() {
  std::tuple<const char*, ip_address> addresses[] = {
    std::make_tuple("127.0.0.1", ip_address(127, 0, 0, 1)),
    std::make_tuple("192.168.11.1", ip_address(192, 168, 11, 1)),
    std::make_tuple("8.8.8.8", ip_address(8, 8, 8, 8)),
    std::make_tuple("127.0.0.1:8080", ip_address(127, 0, 0, 1, 8080))
  };

  char buffer[128];
  for (auto&& addr : addresses)
  {
    ip_address expected_ip;
    const char* ip_string;
    std::tie(ip_string, expected_ip) = addr;
    ip_address ip = parse_ip_address(ip_string);
    sprintf(buffer, "expected %s", ip_string);
    ASSERT_EQm(buffer, expected_ip, ip);
  }

  PASS();
}

SUITE(ip_address_tests) { RUN_TEST(ipv4_addresses_parse_correctly); }
