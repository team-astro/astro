/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_IP_ADDRES_H
#define ASTRO_NET_IP_ADDRES_H

#include <astro/astro.h>
#include <astro/string.h>
#include <string>

namespace astro { namespace net
{
  enum class address_family
  {
    none = -1,
    unspecified = 0,
    unix = 1,
    inter_network = 2,
    imp_link = 3,
    pup = 4,
    chaos = 5,
    ns = 6,
    ipx = 6,
    iso = 7,
    osi = 7,
    ecma = 8,
    datakit = 9,
    ccitt = 10,
    sna = 11,
    dec_net = 12,
    data_link = 13,
    lat = 14,
    hyper_channel = 15,
    apple_talk = 16,
    net_bios = 17,
    voice_view = 18,
    firefox = 19,
    banyan = 21,
    atm = 22,
    inter_network_v6 = 23,
    cluster = 24,
    ieee_12844 = 25,
    irda = 26,
    network_designers = 28,
    max = 29
  };

  struct ip_address
  {
    address_family family;
    uint32 scope;
    union
    {
      uint16 addr_v6[8];
      uint8 addr[16];
    };

    static ip_address loopback;
    static ip_address any;
    static ip_address broadcast;

    static ip_address node_local;
    static ip_address link_local;

    ip_address(address_family family = address_family::none);
    ip_address(uint32 ip);
    ip_address(uint8 a, uint8 b, uint8 c, uint8 d);
    ip_address(uint16 a, uint16 b, uint16 c, uint16 d, uint16 e, uint16 f, uint16 g, uint16 h, uint32 scope = 0);
    ip_address(const ip_address& rhs);

    bool operator==(const ip_address& rhs);
    bool operator!=(const ip_address& rhs);
  };

  uint32
  ip_address_as_v4_int(ip_address* ip);

  ip_address
  parse_ip_address(const char* ip, uint16* port = nullptr);

  const char*
  ip_address_to_string(ip_address* ip, uint16 port = 0, allocator* allocator = default_allocator);

  const char*
  ip_address_to_string(char* buffer, uintptr buffer_len, ip_address* ip, uint16 port = 0);
}}

#endif
