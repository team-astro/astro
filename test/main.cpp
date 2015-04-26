/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <stdio.h>

#define ASTRO_IMPLEMENTATION
#include <astro/astro.h>
#include <astro/memory.h>
#include <astro/io/path.h>

#include <greatest/greatest.h>
GREATEST_MAIN_DEFS();

#include "path_tests.h"

int main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();      /* init & parse command-line args */
  RUN_SUITE(path_tests);
  GREATEST_MAIN_END();        /* display results */
}
