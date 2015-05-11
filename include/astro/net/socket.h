/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_SOCKET_H
#define ASTRO_NET_SOCKET_H

#include <astro/astro.h>
#include <astro/string.h>
#include <stdarg.h>
#include "ip_address.h"

namespace astro { namespace net
{
  enum class socket_type
  {
    unknown = -1,
    stream = 1,
    datagram = 2,
    raw = 3,
    rdm = 4,
    seq_packet = 5
  };

  enum class protocol_type
  {
    unknown = -1,
    unspecified = 0,
    ip = 0,
    ipv6_hop_by_hop_options = 0,
    icmp = 1,
    igmp = 2,
    ggp = 3,
    ipv4 = 4,
    tcp = 6,
    pup = 12,
    udp = 17,
    idp = 22,
    ipv6 = 41,
    ipv6_routing_header = 43,
    ipv6_fragment_header = 44,
    ipsec_encapsulating_security_payload = 50,
    ipsec_authentication_header = 51,
    icmpv6 = 58,
    ipv6_no_next_header = 59,
    ipv6_destination_options = 60,
    nd = 77,
    raw = 255,
    ipx = 1000,
    spx = 1256,
    spxii = 1257,
  };

  struct socket
  {
    int s;
    socket_type type;
    bool32 is_bound;
    bool32 is_listening;
    bool32 is_connected;
    ip_address ip;
    address_family family;
    uint16 port;
  };

  // TODO: Implement non-blocking API.

  socket
  socket_create(address_family family, socket_type type, protocol_type protocol);

  void
  socket_destroy(socket* sock);

  bool32
  socket_bind(socket* s, ip_address ip, uint16 port);

  bool32
  socket_listen(socket* s, int32 backlog);

  bool32
  socket_connect(socket* s, ip_address ip, uint16 port);

  socket
  socket_accept(socket* s);

  void
  socket_close(socket* s);

  bool32
  socket_send(socket* s, uint8* data, uintptr data_len);

  int32
  socket_recv(socket* s, uint8* data, uintptr data_len);
}}

#endif
