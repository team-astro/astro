/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/ip_address.h>

using namespace astro;
using namespace astro::net;

TEST ipv4_addresses_roundtrip_correctly() {
  std::tuple<const char*, ip_address, uint16> addresses[] = {
    std::make_tuple("127.0.0.1", ip_address(127, 0, 0, 1), 0),
    std::make_tuple("192.168.11.1", ip_address(192, 168, 11, 1), 0),
    std::make_tuple("8.8.8.8", ip_address(8, 8, 8, 8), 0),
    std::make_tuple("127.0.0.1:8080", ip_address(127, 0, 0, 1), 8080)
  };

  for (auto&& addr : addresses)
  {
    ip_address test_ip;
    uint16 test_port;
    const char* test_string;
    std::tie(test_string, test_ip, test_port) = addr;

    uint16 result_port;
    ip_address result_ip = parse_ip_address(test_string, &result_port);
    const char* result_string = ip_address_to_string(&result_ip, result_port);

    ASSERT_EQ(test_ip, result_ip);
    ASSERT_EQ(test_port, result_port);
    ASSERT_STR_EQ(test_string, result_string);
  }

  PASS();
}

TEST ipv6_addresses_roundtrip_correctly() {
  std::tuple<const char*, ip_address, uint16> addresses[] = {
    std::make_tuple("fe80::34f7:a7ff:fe43:dd1d%49", ip_address(65152, 0, 0, 0, 13559, 43007, 65091, 56605, 49), 0),
    std::make_tuple("fe80::34f7:1:2:3%4", ip_address(65152, 0, 0, 0, 13559, 1, 2, 3, 4), 0),
    std::make_tuple("::1", ip_address(0, 0, 0, 0, 0, 0, 0, 1), 0),
    std::make_tuple("::", ip_address(0, 0, 0, 0, 0, 0, 0, 0), 0),
    std::make_tuple("::ffff:192.168.1.1", ip_address(0, 0, 0, 0, 0, 65535, 43200, 257), 0),
    std::make_tuple("[::1]:8080", ip_address(0, 0, 0, 0, 0, 0, 0, 1), 8080),
  };

  for (auto&& addr : addresses)
  {
    ip_address test_ip;
    uint16 test_port;
    const char* test_string;
    std::tie(test_string, test_ip, test_port) = addr;

    uint16 result_port;
    ip_address result_ip = parse_ip_address(test_string, &result_port);
    const char* result_string = ip_address_to_string(&result_ip, result_port);

    ASSERT_EQ(test_ip, result_ip);
    ASSERT_EQ(test_port, result_port);
    ASSERT_STR_EQ(test_string, result_string);
  }

  PASS();
}

SUITE(ip_address_tests)
{
  RUN_TEST(ipv4_addresses_roundtrip_correctly);
  RUN_TEST(ipv6_addresses_roundtrip_correctly);
}
