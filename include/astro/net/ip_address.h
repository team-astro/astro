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
  enum class ip_family
  {
    none,
    inter_network,
    inter_network_v6
  };

  struct ip_address
  {
    ip_family family;
    union
    {
      uint16 addr_v6[8];
      uint8 addr[16];
    };
    uint32 zone;

    ip_address(ip_family family = ip_family::none)
      : family(family)
      , zone(0)
    {
      memset(addr, 0, sizeof(uint8) * ASTRO_COUNTOF(addr));
    }

    ip_address(uint32 ip)
      : family(ip_family::inter_network)
      , zone(0)
    {
      memset(addr, 0, sizeof(uint8) * ASTRO_COUNTOF(addr));

      addr[0] = (uint8)((ip >> 0) & 0xFF);
      addr[1] = (uint8)((ip >> 8) & 0xFF);
      addr[2] = (uint8)((ip >> 16) & 0xFF);
      addr[3] = (uint8)((ip >> 24) & 0xFF);
    }

    ip_address(uint8 a, uint8 b, uint8 c, uint8 d)
      : family(ip_family::inter_network)
      , zone(0)
    {
      memset(addr, 0, sizeof(uint8) * ASTRO_COUNTOF(addr));

      addr[0] = a;
      addr[1] = b;
      addr[2] = c;
      addr[3] = d;
    }

    ip_address(uint16 a, uint16 b, uint16 c, uint16 d, uint16 e, uint16 f, uint16 g, uint16 h, uint32 zone = 0)
      : family(ip_family::inter_network_v6)
      , zone(zone)
    {
      memset(addr_v6, 0, sizeof(uint16) * ASTRO_COUNTOF(addr_v6));

      addr_v6[0] = a;
      addr_v6[1] = b;
      addr_v6[2] = c;
      addr_v6[3] = d;
      addr_v6[4] = e;
      addr_v6[5] = f;
      addr_v6[6] = g;
      addr_v6[7] = h;
    }

    ip_address(const ip_address& rhs)
      : zone(rhs.zone)
      , family(rhs.family)
    {
      memcpy(addr, rhs.addr, sizeof(uint8) * ASTRO_COUNTOF(addr));
    }

    bool operator==(const ip_address& rhs)
    {
      return memcmp(addr, rhs.addr, sizeof(uint16) * ASTRO_COUNTOF(addr))
        && family == rhs.family;
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
    parse_hexadecimal(const char** value)
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

    inline uint32
    ip_address_as_v4_int(ip_address* ip)
    {
      uint16* addr = ip->addr_v6;
#if 1
      uint16 upper = addr[7];
      uint16 lower = addr[6];
#else
      uint16 upper = ((addr[7] >> 8) & 0xFF) | ((addr[7] << 8) & 0xFF00);
      uint16 lower = ((addr[6] >> 8) & 0xFF) | ((addr[6] << 8) & 0xFF00);
#endif

      return (upper << 16) | (lower << 0);
    }

    inline bool
    ip_address_is_v4_mapped(ip_address* ip)
    {
      uint16* addr = ip->addr_v6;
      for (int i = 0; i < 5; ++i)
      {
        if (addr[i] != 0)
          return false;
      }

      return addr[5] == 0xffff;
    }
  }

  inline ip_address
  parse_ip_address(const char** ip, uint16* port = nullptr)
  {
    ip_address result = {};
    const char* firstColon = strchr(*ip, ':');
    const char* firstDot = strchr(*ip, '.');
    const char* openBracket = strchr(*ip, '[');
    const char* closeBracket = nullptr;
    const char* ipLocal = *ip;

    bool32 isIPv6Local = openBracket != nullptr || firstDot == nullptr ||
      (firstColon != nullptr && (firstDot == nullptr || firstColon < firstDot));

    if (isIPv6Local)
    {
      closeBracket = strchr(ipLocal, ']');
      if (openBracket != nullptr &&
        (closeBracket == nullptr || closeBracket < openBracket))
        return result;

      bool32 v4_detected = false;
      const char* cursor = nullptr;
      uint8* zeros = nullptr;
      uint8* addrCursor = result.addr;

      if (openBracket)
        ipLocal = openBracket + 1;

      int index = 0;
      for (; index < 8; ++index)
      {
        cursor = ipLocal;
        uint32 value = parse_hexadecimal(&ipLocal);
        if (cursor == ipLocal)
        {
          if (zeros)
          {
            if (zeros == addrCursor)
            {
              --index;
              break;
            }

            return {};
          }

          if (*ipLocal != ':')
            return {};

          if (index == 0)
          {
            ++(ipLocal);
            if (*ipLocal != ':')
              return {};
          }

          zeros = addrCursor;
          ++(ipLocal);
        }
        else
        {
          if (*ipLocal == '.') // internal ipv4
          {
            const char* localCursor = cursor;
            ip_address ip_local = parse_ip_address(&localCursor);

            if (ip_local.family != ip_family::inter_network)
              return {};

            *(addrCursor++) = ip_local.addr[0];
            *(addrCursor++) = ip_local.addr[1];
            *(addrCursor++) = ip_local.addr[2];
            *(addrCursor++) = ip_local.addr[3];

            ++index;
            v4_detected = true;
            break;
          }

          if (value > 65535)
            return {};

          // TODO: Check endianness.
          *(addrCursor++) = value & 0xFF;
          *(addrCursor++) = value >> 8;

          if (*ipLocal == ':')
          {
            ++(ipLocal);
          }
          else
          {
            break;
          }
        }
      }

      if (zeros)
      {
        int head = (int)(zeros - result.addr);
        int tail = (index * 2) - ((int)(zeros - result.addr));
        int zeroCount = 16 - tail - head;
        memmove(&result.addr[16 - tail], zeros, tail);
        memset(zeros, 0, zeroCount);
      }

      if (*ipLocal == '%')
      {
        ++(ipLocal);
        const char* textBefore = ipLocal;
        uint32 zone = parse_decimal(&ipLocal);

        if (textBefore != ipLocal)
          result.zone = zone;
      }

      if (v4_detected)
      {
        static const uint8 v4Prefix[] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0xff,0xff };
        if (0 != memcmp(result.addr, v4Prefix, ASTRO_COUNTOF(v4Prefix)))
          return {};
      }

      if (openBracket)
      {
        if (*ipLocal != ']')
          return {};
        ++(ipLocal);
      }

      if (*ipLocal == ':' && port)
      {
        const char* textBefore;
        uint32 value;
        ++(ipLocal);
        textBefore = ipLocal;
        value = parse_decimal(&ipLocal);

        if (value <= 65535 && textBefore != ipLocal)
          *port = (uint16) value;
      }
      else if (port)
      {
        *port = 0;
      }

      result.family = ip_family::inter_network_v6;
    }
    else
    {
      if (firstDot == nullptr ||
        (firstColon != nullptr && firstColon < firstDot))
        return result;

      uint32 value = 0;
      const char* cursor;
      uint8* addrCursor = result.addr;

      for (int i = 0; i < 3; ++i)
      {
        cursor = ipLocal;
        value = parse_decimal(&ipLocal);
        if (*ipLocal != '.' || value > 255 || cursor == ipLocal)
          return {};

        *(addrCursor++) = (uint8)value;
        ++(ipLocal);
      }

      cursor = ipLocal;
      value = parse_decimal(&ipLocal);
      if (value > 255 || cursor == ipLocal)
        return {};

      *(addrCursor++) = (uint8)value;

      result.family = ip_family::inter_network;
      if (port)
      {
        *port = 0;
        if (*ipLocal == ':')
        {
          ++(ipLocal);

          cursor = ipLocal;
          value = parse_decimal(&ipLocal);
          if (value > 65535 || cursor == ipLocal)
            return result;

          *port = (uint16) value;
        }
      }
    }

    return result;
  }

  template <typename Allocator = allocator<char>>
  inline const char*
  ip_address_to_string(ip_address* ip, uint16 port = 0, Allocator allocator = Allocator())
  {
    const char* result = nullptr;
    if (ip->family == ip_family::inter_network)
    {
      uint8* addr = ip->addr;
      if (port > 0)
      {
        result = saprintf("%u.%u.%u.%u:%u", allocator, addr[0], addr[1], addr[2], addr[3], port);
      }
      else
      {
        result = saprintf("%u.%u.%u.%u", allocator, addr[0], addr[1], addr[2], addr[3]);
      }
    }
    else if (ip->family == ip_family::inter_network_v6)
    {
      uint16* addr = ip->addr_v6;
      bool32 is_ipv4_mapped = ip_address_is_v4_mapped(ip);

      constexpr uintptr buffer_len = 45;
      char buffer[buffer_len] = {};
      char digit_buf[12]; // room for uint32 max digits + a character + null term.

      if (is_ipv4_mapped)
      {
        ip_address ipv4(ip_address_as_v4_int(ip));
        snprintf(buffer, buffer_len, "::ffff:%u.%u.%u.%u", ipv4.addr[0],
          ipv4.addr[1], ipv4.addr[2], ipv4.addr[3]);
      }
      else
      {
        int bestStart = -1;
        int bestLen = 0;
        int currentLen = 0;

        uint16* addr = ip->addr_v6;
        for (int i = 0; i < 8; ++i)
        {
          if (addr[i] != 0)
          {
            if (currentLen > bestLen && currentLen > 1)
            {
              bestLen = currentLen;
              bestStart  = i - currentLen;
            }
            currentLen = 0;
          }
          else
          {
            ++currentLen;
          }
        }

        if (currentLen > bestLen && currentLen > 1)
        {
          bestLen = currentLen;
          bestStart = 8 - currentLen;
        }

        if (bestStart == 0)
          strlcat(buffer, ":", buffer_len);

        for (int i = 0; i < 8; ++i)
        {
          if (bestStart == i)
          {
            strlcat(buffer, ":", buffer_len);
            i += bestLen - 1;
            continue;
          }

          snprintf(digit_buf, ASTRO_COUNTOF(digit_buf), "%x%s", addr[i], i < 7 ? ":" : "");
          strlcat(buffer, digit_buf, buffer_len);
        }
      }

      if (ip->zone != 0)
      {
        snprintf(digit_buf, ASTRO_COUNTOF(digit_buf), "%%%u", ip->zone);
        strlcat(buffer, digit_buf, buffer_len);
      }

      if (port > 0)
      {
        result = saprintf("[%s]:%u", allocator, buffer, port);
      }
      else
      {
        result = strdup(buffer, allocator);
      }
    }

    return result;
  }
}}

#endif
