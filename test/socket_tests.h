/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/socket.h>

using namespace astro;
using namespace astro::net;

#include <chrono>
#include <thread>

using namespace std::chrono;

TEST socket_should_listen_and_connect()
{
  auto client = socket_create(address_family::inter_network,
    socket_type::stream, protocol_type::tcp);

  bool32 server_up = false;
  std::thread server_thread([](bool32* listening)
  {
    auto server = socket_create(address_family::inter_network,
      socket_type::stream, protocol_type::tcp);
    ASSERT(socket_bind(&server, ip_address::loopback, 54321));
    ASSERT(socket_listen(&server, 10));

    *listening = true;
    
    auto connection = socket_accept(&server);
    socket_close(&connection);
    socket_close(&server);

    PASS();
  }, &server_up);

  // TODO: Better spinwait?
  while (!server_up)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  
  ASSERT(socket_connect(&client, ip_address::loopback, 54321));

  server_thread.join();

  PASS();
}

SUITE(socket_tests)
{
  RUN_TEST(socket_should_listen_and_connect);
};
