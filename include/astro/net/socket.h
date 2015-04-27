/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_SOCKET_H
#define ASTRO_NET_SOCKET_H

#include <astro/astro.h>
#include <astro/string.h>
#include <stdarg.h>

namespace astro { namespace net
{
  struct socket
  {
    static socket create();
  };
}}


#if ASTRO_PLATFORM_POSIX
#include "posix/socket.inl"
#else
#error "astro::net::socket not implemented."
#endif

#endif
