/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <stdio.h>

#define ASTRO_IMPLEMENTATION
#include <astro/astro.h>
#include <astro/memory.h>

#include <greatest/greatest.h>
GREATEST_MAIN_DEFS();

#include "path_tests.h"
#include "socket_tests.h"
#include "ip_address_tests.h"

int main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();      /* init & parse command-line args */
  RUN_SUITE(path_tests);
  RUN_SUITE(socket_tests);
  RUN_SUITE(ip_address_tests);
  GREATEST_MAIN_END();        /* display results */
}
