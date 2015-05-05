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

#ifdef ASTRO_IMPLEMENTATION
#include <http-parser/http_parser.c>
#endif

namespace astro { namespace net
{
  enum class http_status_code : uint16
  {
    /** Informational */
    Continue = 100,
    SwitchingProtocols = 101,
    Processing = 102,

    /** Success */
    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInfo = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,
    AlreadyReported = 208,
    IMUsed = 226,

    /** Redirection */
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    SwitchProxy = 306,
    TemporaryRedirect = 307,
    PermanantRedirect = 308,

    /** Client error */
    BadReqest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequire = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    RequestEntityToLarge = 413,
    RequestUriTooLong = 414,
    UnsupportedMediaType = 415,
    RequestedRangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    UnprocessableEntity = 422,
    Locked = 423,
    FailedDependency = 424,
    UpgradeRequired = 426,

    /** Server error */
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    LoopDetected = 508,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511
  };

  struct http_response
  {
    http_status_code status_code;
  };

  typedef std::pair<const char*, const char*> http_header;
  typedef std::pair<const char*, const char*> http_query_param;

  std::future<http_response>
  http_request(const char* url, const char* method = "GET");

#ifdef ASTRO_IMPLEMENTATION
  namespace
  {
    struct http_response_in_flight
    {
      http_response* response;
      socket* socket;
    };

    inline http_status_code
    parse_status_code(const char* value, size_t length)
    {
      uint16 result = 0;
      uint8 digit;
      while (digit = *value, digit >= '0' && digit <= '9' && length > 0)
      {
        result *= 10;
        result += digit - '0';

        ++value;
        --length;
      }

      return (http_status_code) result;
    }

    static int on_message_begin(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_message_begin");
      return 0;
    }

    static int on_headers_complete(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_headers_complete");
      return 0;
    }

    static int on_message_complete(http_parser* p)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_message_complete");
      return 0;
    }

    static int on_header_field(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_header_field: %*s", (int) length, at);
      return 0;
    }

    static int on_header_value(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_header_value: %*s", (int) length, at);
      return 0;
    }

    static int on_url(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      log_debug("on_url: %*s", (int) length, at);
      return 0;
    }

    static int on_status(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      r->response->status_code = parse_status_code(at, length);
      log_debug("on_status: %*s", (int) length, at);
      return 0;
    }

    static int on_body(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_response_in_flight*) p->data;
      //log_debug("on_body: %*s", (int) length, at);
      return 0;
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

          if (seg_port.len > 0)
            sscanf(url + seg_port.off, "%hd", &port);

          const char* host = url + seg_host.off;
          auto ips = dns::resolve_host_name(host, seg_host.len, address_family::inter_network).get();
          for (auto& ip : ips)
          {
            ip_address_to_string(buffer, sizeof(buffer), &ip);
            log_debug("resolved to %s", buffer);

            socket conn = socket_create(ip.family, socket_type::stream, protocol_type::tcp);
            if (socket_connect(&conn, ip, port))
            {
              // GET /path HTTP/1.1\r\n
              // <headers>\r\n
              // \r\n
              // <body>
              constexpr uintptr nhdr = HTTP_MAX_HEADER_SIZE;
              char hdr[nhdr];
              uintptr msg_len = 0;
              msg_len += append_format(hdr, nhdr,
                "%s %*s HTTP 1.1\r\nUser-Agent: curl/7.37.1\r\nHost: %*s\r\nAccept: */*\r\n\r\n",
                method, seg_path.len == 0 ? 1 : seg_path.len,
                seg_path.len == 0 ? "/" : url + seg_path.off,
                seg_host.len, url + seg_host.off);

              log_debug("Sending request:\n%*s:len -> %ld", (int)msg_len - 1, hdr, msg_len);

              socket_send(&conn, (uint8*) hdr, msg_len - 1);
              http_parser parser = {};
              http_parser_init(&parser, HTTP_RESPONSE);

              http_response_in_flight res_in;
              res_in.response = &res;
              res_in.socket = &conn;
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
              // TODO: Read response.
              while ((bytes_recvd = socket_recv(&conn, (uint8*) buf, BUFSIZ)) > 0)
              {
                size_t parsed = http_parser_execute(&parser, &settings, buf, bytes_recvd);
                if (parsed == 0)
                  break;
              }

              if (conn.is_connected)
              {
                socket_close(&conn);
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
