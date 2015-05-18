/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/socket.h>

using namespace astro;
using namespace astro::net;

#include <chrono>
#include <thread>

#define INET6_ADDRSTRLEN 48

using namespace std::chrono;

constexpr uint32 test_port = 54321;

TEST socket_should_listen_and_connect()
{
  auto client = socket_create(address_family::inter_network,
    socket_type::stream, protocol_type::tcp);

  bool32 server_up = false;
  std::thread server_thread([](bool32* listening)
  {
    auto server = socket_create(address_family::inter_network,
      socket_type::stream, protocol_type::tcp);
    ASSERT(socket_bind(&server, ip_address::loopback, test_port));
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

  ASSERT(socket_connect(&client, ip_address::loopback, test_port));

  server_thread.join();

  socket_close(&client);

  PASS();
}

TEST socket_should_send_and_recv()
{
  auto client = socket_create(address_family::inter_network,
    socket_type::stream, protocol_type::tcp);

  bool32 server_up = false;
  std::thread server_thread([](bool32* listening)
  {
    auto server = socket_create(address_family::inter_network,
      socket_type::stream, protocol_type::tcp);
      socket_bind(&server, ip_address::loopback, test_port);
    socket_listen(&server, 10);

    *listening = true;

    auto connection = socket_accept(&server);

    char ip[INET6_ADDRSTRLEN + 7];
    ip_address_to_string(ip, sizeof(ip), &connection.ip, connection.port);

    char buffer[256];
    sprintf(buffer + 1, "Hello, %s", ip);
    char buffer_len = strlen(buffer + 1) + 1;
    buffer[0] = buffer_len;

    ASSERT(socket_send(&connection, (uint8*) buffer, buffer_len + 1));

    ASSERT_EQ(sizeof(char), socket_recv(&connection, (uint8*) &buffer_len, sizeof(char)));
    ASSERT_EQ(buffer_len, socket_recv(&connection, (uint8*) buffer, buffer_len));
    log_debug("server recv: %s", buffer);

    socket_close(&connection);
    socket_close(&server);

    PASS();
  }, &server_up);

  // TODO: Better spinwait?
  while (!server_up)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  socket_connect(&client, ip_address::loopback, test_port);

  char ip[INET6_ADDRSTRLEN + 7];
  ip_address_to_string(ip, sizeof(ip), &client.ip, client.port);

  char buffer[256];
  char buffer_len = 0;

  ASSERT_EQ(sizeof(char), socket_recv(&client, (uint8*) &buffer_len, sizeof(char)));
  ASSERT_EQ(buffer_len, socket_recv(&client, (uint8*) buffer, buffer_len));
  log_debug("client recv: %s", buffer);

  sprintf(buffer + 1, "Hello, %s", ip);
  buffer_len = strlen(buffer + 1) + 1;
  buffer[0] = buffer_len;

  ASSERT(socket_send(&client, (uint8*) buffer, buffer_len + 1));

  server_thread.join();

  socket_close(&client);

  PASS();
}

SUITE(socket_tests)
{
  RUN_TEST(socket_should_listen_and_connect);
  RUN_TEST(socket_should_send_and_recv);
};
