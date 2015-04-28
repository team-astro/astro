/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/socket.h>

using namespace astro;
using namespace astro::net;

TEST socket_should_listen_and_connect()
{
  auto client = socket_create(address_family::inter_network,
    socket_type::stream, protocol_type::tcp);

  std::async(std::launch::async, []()
  {
    auto server = socket_create(address_family::inter_network,
      socket_type::stream, protocol_type::tcp);
    ASSERT(socket_bind(&server, ip_address::loopback, 54321));
    ASSERT(socket_listen(&server, 10));

    auto connection = socket_accept(&server);
    socket_close(&connection);
    socket_close(&server);

    PASS();
  });

  ASSERT(socket_connect(&client, ip_address::loopback, 54321));

  PASS();
}

SUITE(socket_tests)
{

  RUN_TEST(socket_should_listen_and_connect);
};
