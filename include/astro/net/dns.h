/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_DNS_H
#define ASTRO_NET_DNS_H

#include <astro/astro.h>
#include <astro/string.h>

#include <tuple>
#include <future>

#include "ip_address.h"

namespace astro { namespace net
{
  namespace dns
  {
    std::future<ip_address>
    resolve_host_name(const char* path);
  }
}}

#if ASTRO_PLATFORM_POSIX
#include "posix/dns.inl"
#else
#error "astro::net::dns not implemented."
#endif

#endif
