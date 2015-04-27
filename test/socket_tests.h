/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/socket.h>

using namespace astro;
using namespace astro::io;

TEST socket_should_connect()
{
  PASS();
}

SUITE(socket_tests) { RUN_TEST(socket_should_connect); };
