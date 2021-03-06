/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_DNS_H
#define ASTRO_NET_DNS_H

#include <astro/astro.h>
#include <astro/string.h>

#include <tuple>
#include <future>
#include <vector>

#include "ip_address.h"

namespace astro { namespace net
{
  namespace dns
  {
    // TODO: Review this api. Still not sure about this...
    std::future<std::vector<ip_address>>
    resolve_host_name(const char* hostname, uintptr hostname_len,
      address_family preferred_family = address_family::none);
  }
}}

#endif
