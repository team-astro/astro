/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_IP_ADDRES_H
#define ASTRO_NET_IP_ADDRES_H

#include <astro/astro.h>
#include <astro/string.h>

namespace astro { namespace net
{
  enum class ip_family
  {
    none,
    inter_network,
    inter_network_v6
  };

  struct ip_address
  {
    uint8 addr[16];
    uint16 port;
    ip_family family;

    ip_address()
      : addr()
      , port(0)
      , family(ip_family::none)
    {
    }

    ip_address(uint8 fst, uint8 snd, uint8 thr, uint8 fth)
      : addr()
      , port(0)
      , family(ip_family::inter_network)

    {
      addr[0] = fst;
      addr[1] = snd;
      addr[2] = thr;
      addr[3] = fth;
    }

    ip_address(uint8 fst, uint8 snd, uint8 thr, uint8 fth, uint16 port)
      : addr()
      , port(port)
      , family(ip_family::inter_network)

    {
      addr[0] = fst;
      addr[1] = snd;
      addr[2] = thr;
      addr[3] = fth;
    }

    bool operator==(const ip_address& rhs)
    {
      return memcmp(addr, rhs.addr, sizeof(uint8) * ASTRO_COUNTOF(addr)) == 0
        && port == rhs.port && family == rhs.family;
    }

    bool operator!=(const ip_address& rhs)
    {
      return !((*this) == rhs);
    }
  };

  namespace
  {
    inline uint32
    parse_decimal(const char** value)
    {
      uint32 result = 0;
      uint8 digit;
      while (digit = **value, digit >= '0' && digit <= '9')
      {
        result *= 10;
        result += digit - '0';

        ++*value;
      }

      return result;
    }

    inline uint32
    parse_hexidecimal(const char** value)
    {
      uint32 result = 0;
      uint8 digit;
      while (digit = **value & 0x5f,
        (digit >= ('0' & 0x5f) && digit <= ('9' & 0x5f)) ||
        (digit >= 'A' && digit <= 'F'))
      {
        digit -= 0x10;
        uint8 digitValue = (digit > 9 ? digit - (0x31 - 0x0a) : digit);
        result <<= 4;
        result += digitValue;

        ++*value;
      }

      return result;
    }

    inline bool
    parse_ipv4(const char* ip, ip_address& out)
    {
      uint32 value = 0;
      const char* cursor;
      uint8* addrCursor = out.addr;

      for (int i = 0; i < 3; ++i)
      {
        cursor = ip;
        value = parse_decimal(&ip);
        if (*ip != '.' || value > 255 || cursor == ip)
          return false;

        *(addrCursor++) = (uint8)value;
        ++ip;
      }

      cursor = ip;
      value = parse_decimal(&ip);
      if (value > 255 || cursor == ip)
        return false;

      *(addrCursor++) = (uint8)value;

      out.family = ip_family::inter_network;
      out.port = 0;
      if (*ip == ':')
      {
        ++ip;

        cursor = ip;
        value = parse_decimal(&ip);
        if (value > 65535 || cursor == ip)
          return false;

        out.port = (uint16) value;

      }

      return true;
    }

    inline bool
    parse_ipv6(const char* ip, ip_address& out)
    {
      // TODO: Parse IPv6
      return false;
    }
  }

  inline ip_address
  parse_ip_address(const char* ip)
  {
    ip_address result = {};
    const char* firstColon = strchr(ip, ':');
    const char* firstDot = strchr(ip, '.');
    const char* openBracket = strchr(ip, '[');
    const char* closeBracket = nullptr;

    bool32 isIPv6Local = openBracket != nullptr || firstDot == nullptr ||
      (firstColon != nullptr && (firstDot == nullptr || firstColon < firstDot));

    if (isIPv6Local)
    {
      closeBracket = strchr(ip, ']');
      if (openBracket != nullptr &&
        (closeBracket == nullptr || closeBracket < openBracket))
        return result;

      if (!parse_ipv6(ip, result))
        result = {};
    }
    else
    {
      if (firstDot == nullptr ||
        (firstColon != nullptr && firstColon < firstDot))
        return result;

      if (!parse_ipv4(ip, result))
        result = {};
    }

    return result;
  }

  template <typename Allocator = allocator<const char>>
  inline const char*
  ip_address_to_string(ip_address ip, Allocator allocator = Allocator())
  {
    const char* result = nullptr;
    if (ip.family == ip_family::inter_network)
    {
      if (ip.port > 0)
        result = saprintf("%c.%c.%c.%c:%o", allocator, ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3], ip.port);
      else
        result = sprintf("%c.%c.%c.%c", allocator, ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3]);
    }
    else if (ip.family == ip_family::inter_network_v6)
    {
      // TODO: format IPv6.
    }

    return result;
  }
}}

#endif
