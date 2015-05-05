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

#ifdef ASTRO_IMPLEMENTATION
#include <http-parser/http_parser.c>
#endif

namespace astro { namespace net
{
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

  struct http_response
  {
    http_status_code status_code;
    std::map<std::string, std::string> headers;
    uint8* body;
  };

  typedef std::pair<const char*, const char*> http_header;
  typedef std::pair<const char*, const char*> http_query_param;

  std::future<http_response>
  http_request(const char* url, const char* method = "GET");

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

#ifdef ASTRO_IMPLEMENTATION
  namespace
  {
    struct http_response_in_flight
    {
      char tmp[256];
      http_response* response;
      socket* socket;

      const char *current_header_field;
      uintptr current_header_field_len;
      uintptr body_written;
    };

    static int on_message_begin(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      return 0;
    }

    static int on_headers_complete(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      if (http_should_keep_alive(p) == 0)
      {
        log_debug("headers complete on non-keep alive socket. closing.");
        socket_close(r->socket);
      }
      else if (p->content_length)
      {
        r->body_written = 0;
        r->response->body = (uint8*) malloc(p->content_length);
      }

      return 0;
    }

    static int on_message_complete(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      if (http_should_keep_alive(p) == 0)
      {
        log_debug("message complete on non-keep alive socket. closing.");
        socket_close(r->socket);
      }

      return 0;
    }

    static int on_header_field(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_header_field: %s", r->tmp);

      r->current_header_field = at;
      r->current_header_field_len = length;

      return 0;
    }

    static int on_header_value(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_header_value: %s", r->tmp);

      if (r->current_header_field && r->current_header_field_len)
      {
        std::string key(r->current_header_field, r->current_header_field_len);
        r->response->headers[key] = std::string(at, length);
      }

      return 0;
    }

    static int on_url(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_url: %s", r->tmp);
      return 0;
    }

    static int on_status(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;

      r->response->status_code = (http_status_code) p->status_code;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_status: %s", r->tmp);
      return 0;
    }

    static int on_body(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;

      // TODO: Handle body streaming.
      uint8* body = r->response->body;
      uintptr body_pos = r->body_written;
      if (body && body_pos + length <= p->content_length)
      {
        memcpy(body + body_pos, at, length);
        r->body_written += length;
      }

      if (http_body_is_final(p) != 0)
      {
        log_debug("on_body: \n%s", body);
      }

      return 0;
    }

    static void
    do_request(ip_address ip, uint16 port, const char* method,
      const char* host, uintptr host_len,
      const char* path, uintptr path_len,
      http_response* res, socket* conn = nullptr)
    {
      bool32 one_use_socket = conn == nullptr;
      socket s = {};
      if (one_use_socket)
      {
        s = socket_create(ip.family, socket_type::stream, protocol_type::tcp);
        conn = &s;
      }

      if (conn->is_connected || socket_connect(conn, ip, port))
      {
        assert(conn->ip == ip);
        char hdr[HTTP_MAX_HEADER_SIZE] = {};
        uintptr msg_len = 0;
        msg_len += append_format(hdr, HTTP_MAX_HEADER_SIZE,
          "%s %*s HTTP/1.1\r\n"
          "Host: %*s\r\n"
          "User-Agent: curl/7.37.1\r\n",
          method,
          path_len, path,
          host_len, host);

        if (!one_use_socket)
        {
          msg_len += append_format(hdr, HTTP_MAX_HEADER_SIZE,
            "Connection: Keep-Alive\r\n");
        }

        msg_len += append_format(hdr, HTTP_MAX_HEADER_SIZE,
          "Accept: */*\r\n"
          "\r\n");

        log_debug("Sending request (%ld bytes):\n\"%*s\"", msg_len, (int)msg_len - 2, hdr);

        socket_send(conn, (uint8*) hdr, msg_len);
        http_parser parser = {};
        http_parser_init(&parser, HTTP_RESPONSE);

        http_response_in_flight res_in;
        res_in.response = res;
        res_in.socket = conn;
        parser.data = &res_in;

        static http_parser_settings settings = {};
        settings.on_message_begin = on_message_begin;
        settings.on_headers_complete = on_headers_complete;
        settings.on_message_complete = on_message_complete;
        settings.on_header_field = on_header_field;
        settings.on_header_value = on_header_value;
        settings.on_url = on_url;
        settings.on_status = on_status;
        settings.on_body = on_body;

        int bytes_recvd = 0;
        char buf[BUFSIZ];

        auto p = &parser;
        while (conn->is_connected)
        {
          if ((bytes_recvd = socket_recv(conn, (uint8*) buf, BUFSIZ)) > 0)
          {
            http_parser_execute(p, &settings, buf, bytes_recvd);

            if (HTTP_PARSER_ERRNO(p) != HPE_OK)
            {
              log_debug("error in http parser. %s: %s",
                http_errno_name(HTTP_PARSER_ERRNO(p)),
                http_errno_description(HTTP_PARSER_ERRNO(p)));
            }
          }
        }
      }
    }
  }
  std::future<http_response>
  http_request(const char* url, const char* method)
  {
    return std::async(std::launch::async,
      [](const char* url, const char* method)
        {
          http_response res = {};
          http_parser_url parsed_url = {};
          http_parser_parse_url(url, strlen(url), false, &parsed_url);

          auto& seg_scheme = parsed_url.field_data[UF_SCHEMA];
          auto& seg_host = parsed_url.field_data[UF_HOST];
          auto& seg_path = parsed_url.field_data[UF_PATH];
          auto& seg_port = parsed_url.field_data[UF_PORT];
          auto& seg_query = parsed_url.field_data[UF_QUERY];
          auto& seg_fragment = parsed_url.field_data[UF_FRAGMENT];
          auto& seg_userinfo = parsed_url.field_data[UF_USERINFO];

          char buffer[1025] = {};
          log_debug("parsed url: %s", url);

#define log_debug_seg(seg) \
          if (seg_##seg.len > 0) { \
            strncpy(buffer, url + seg_##seg.off, seg_##seg.len); \
            buffer[seg_##seg.len] = '\0'; \
            log_debug(#seg ": %s", buffer); \
          }

          log_debug_seg(scheme);
          log_debug_seg(host);
          log_debug_seg(path);
          log_debug_seg(port);
          log_debug_seg(query);
          log_debug_seg(fragment);
          log_debug_seg(userinfo);

#undef log_debug_seg
#define url_seg(seg) url + seg_##seg.off

          uint16 port = 0;

          // TODO: Lookup table!
          if (strncmp("https", url_seg(scheme), 5) == 0)
            port = 443;
          else if (strncmp("http", url_seg(scheme), 4) == 0)
            port = 80;

          if (parsed_url.port > 0)
            port = parsed_url.port;

          const char* host = url + seg_host.off;
          auto ips = dns::resolve_host_name(host, seg_host.len, address_family::inter_network).get();
          for (auto& ip : ips)
          {
            ip_address_to_string(buffer, sizeof(buffer), &ip);
            log_debug("resolved to %s", buffer);

            const char* host = url + seg_host.off;
            uintptr host_len = seg_host.len;
            const char* path = "/";
            uintptr path_len = 1;
            if (seg_path.len > 0)
            {
              path_len = seg_path.len;
              path = url + seg_path.off;
            }

            socket conn = socket_create(ip.family, socket_type::stream, protocol_type::tcp);
            do_request(ip, port, method, host, host_len, path, path_len, &res, &conn);

            // TODO: Make an option for this functionality.
            http_status_code status = res.status_code;
            while (http_status_code_is_redirect(status))
            {
              auto location = res.headers.find("Location");
              if (location != res.headers.end())
              {
                do_request(ip, port, method, host, host_len,
                  location->second.c_str(), location->second.size(), &res);

                // Check the status code again, in case of chaining redirects.
                status = res.status_code;
              }
              else
              {
                break;
              }
            }


          }

          return res;
        }, url, method);
  }
#endif

}}
#endif
