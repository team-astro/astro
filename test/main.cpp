/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <stdio.h>

#define ASTRO_IMPLEMENTATION
#include <astro/astro.h>
#include <astro/memory.h>
#include <astro/logging.h>
#include <astro/io/directory.h>
#include <astro/io/file.h>
#include <astro/io/path.h>
#include <astro/net/dns.h>
#include <astro/net/http_client.h>
#include <astro/net/ip_address.h>
#include <astro/net/socket.h>
#undef ASTRO_IMPLEMENTATION

astro::log_level astro_log_verbosity = astro::log_level::none;

#include <greatest/greatest.h>
GREATEST_MAIN_DEFS();

#include "path_tests.h"
#include "socket_tests.h"
#include "ip_address_tests.h"
#include "dns_tests.h"
#include "http_client_tests.h"

int main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();      /* init & parse command-line args */

  if (greatest_info.flags & GREATEST_FLAG_VERBOSE)
    astro_log_verbosity = astro::log_level::debug;

  RUN_SUITE(path_tests);
  RUN_SUITE(socket_tests);
  RUN_SUITE(ip_address_tests);
  //RUN_SUITE(dns_tests);
  RUN_SUITE(http_client_tests);
  GREATEST_MAIN_END();        /* display results */
}
