/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_NET_HTTP_CLIENT_H
#define ASTRO_NET_HTTP_CLIENT_H

#include <algorithm>
#include <astro/astro.h>
#include <astro/string.h>
#include "dns.h"
#include "ip_address.h"
#include "socket.h"
#include <map>

// HTTP Client to-do list
// Currently supported:
// - GET requests
// - Read headers to std::map
// - Following redirects
// Bugs:
// - Body reads stop part way through
// Todos:
// - POST forms, content (JSON, etc)
// - Cookie handling
// - Caching
// - Loads more

namespace astro { namespace net
{
  constexpr uint8 kMaxHttpRedirects = 4;

  enum class http_status_code : uint16
  {
    none = 0,

    /** Informational */
    continue_request = 100, // Actually continue, but that's a keyword. :(
    switching_protocols = 101,
    processing = 102,

    /** Success */
    ok = 200,
    created = 201,
    accepted = 202,
    non_authoritative_info = 203,
    no_content = 204,
    reset_content = 205,
    partial_content = 206,
    multi_status = 207,
    already_reported = 208,
    im_used = 226,

    /** Redirection */
    multiple_choices = 300,
    moved_permanently = 301,
    found = 302,
    see_other = 303,
    not_modified = 304,
    use_proxy = 305,
    switch_proxy = 306,
    temporary_redirect = 307,
    permanant_redirect = 308,

    /** Client error */
    bad_request = 400,
    unauthorized = 401,
    payment_required = 402,
    forbidden = 403,
    not_found = 404,
    method_not_allowed = 405,
    not_acceptable = 406,
    proxy_authentication_required = 407,
    request_timeout = 408,
    conflict = 409,
    gone = 410,
    length_required = 411,
    precondition_failed = 412,
    request_entity_to_large = 413,
    request_uri_too_long = 414,
    unsupported_media_type = 415,
    requested_range_not_satisfiable = 416,
    expectation_failed = 417,
    unprocessable_entity = 422,
    locked = 423,
    failed_dependency = 424,
    upgrade_required = 426,

    /** Server error */
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503,
    gateway_timeout = 504,
    http_version_not_supported = 505,
    variant_also_negotiates = 506,
    insufficient_storage = 507,
    loop_detected = 508,
    not_extended = 510,
    network_authentication_required = 511,

    max
  };

  struct http_header
  {
    const char* key;
    const char* value;
    uintptr key_length;
    uintptr value_length;
    http_header* next;
  };

  struct http_request
  {
    const char* url;
    const char* method;
    socket* socket;
  };

  struct http_response
  {
    http_status_code status_code;
    http_header* headers;
    uint8* body;
    uintptr body_len;
  };

  //typedef std::pair<const char*, const char*> http_header;
  //typedef std::pair<const char*, const char*> http_query_param;

  std::future<http_response>
  http(http_request* req, allocator* alloc = default_allocator);

  inline bool32
  http_status_code_is_success(http_status_code status)
  {
    return status >= http_status_code::ok &&
      status < http_status_code::multiple_choices;
  }

  inline bool32
  http_status_code_is_redirect(http_status_code status)
  {
    return status >= http_status_code::multiple_choices &&
      status < http_status_code::bad_request;
  }

  inline bool32
  http_status_code_is_client_error(http_status_code status)
  {
    return status >= http_status_code::bad_request &&
      status < http_status_code::internal_server_error;
  }

  inline bool32
  http_status_code_is_server_error(http_status_code status)
  {
    return status >= http_status_code::internal_server_error &&
      status < http_status_code::max;
  }

  inline http_header*
  http_header_find_key(http_header* headers, const char* key)
  {
    size_t key_length = strlen(key);
    while (headers)
    {
      size_t max_len = max(key_length, headers->key_length);
      if (strncmp(key, headers->key, max_len) == 0)
        return headers;
      headers = headers->next;
    }

    return nullptr;
  }
}}
#endif
