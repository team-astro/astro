/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_DNS_H
#define ASTRO_NET_DNS_H

#include <astro/astro.h>
#include <astro/string.h>
#include "ip_address.h"
#include <tuple>

namespace astro { namespace net
{
  struct ip_address
  {

  };

  namespace dns
  {
    static std::promise<std::tuple<ip_address*, uintptr>>
    resolve_host_name(const char* path);
  }
}}


#if ASTRO_PLATFORM_POSIX
#include "posix/file.inl"
#else
#error "astro::io::file not implemented."
#endif

#endif
