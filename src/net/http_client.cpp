/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/astro.h>
#include <astro/logging.h>

#include <astro/net/http_client.h>
#include <http-parser/http_parser.h>

namespace astro { namespace net
{
  namespace
  {
    struct http_request_closure
    {
      char tmp[256];

      http_request* request;
      http_response* response;
      allocator* allocator;

      const char *current_header_field;
      uintptr current_header_field_len;
      uintptr body_written;
    };

    static int on_message_begin(http_parser*)
    {
      //auto r = (http_request_closure*) p->data;
      return 0;
    }

    static int on_headers_complete(http_parser* p)
    {
      auto r = (http_request_closure*) p->data;
      auto req = r->request;
      if (http_should_keep_alive(p) == 0)
      {
        log_debug("headers complete on non-keep alive socket. closing.");
        socket_close(req->socket);
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
      auto r = (http_request_closure*) p->data;
      auto req = r->request;
      if (http_should_keep_alive(p) == 0)
      {
        log_debug("message complete on non-keep alive socket. closing.");
        socket_close(req->socket);
      }

      return 0;
    }

    static int on_header_field(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_request_closure*) p->data;
      auto res = r->response;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_header_field: %s", r->tmp);

      r->current_header_field = at;
      r->current_header_field_len = length;

      http_header* current = (http_header*) ASTRO_ALLOC(r->allocator, sizeof(http_header));
      current->next = res->headers;
      auto key = (char*) ASTRO_ALLOC(r->allocator, length + 1);
      strncpy(key, at, length);
      key[length] = '\0';
      current->key = key;
      current->key_length = length;

      res->headers = current;

      return 0;
    }

    static int on_header_value(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_request_closure*) p->data;
      auto res = r->response;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_header_value: %s", r->tmp);

      astro_assert(res->headers);
      auto value = (char*) ASTRO_ALLOC(r->allocator, length + 1);
      strncpy(value, at, length);
      value[length] = '\0';
      res->headers->value = value;
      res->headers->value_length = length;

      return 0;
    }

    static int on_url(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_request_closure*) p->data;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_url: %s", r->tmp);
      return 0;
    }

    static int on_status(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_request_closure*) p->data;

      r->response->status_code = (http_status_code) p->status_code;
      strncpy(r->tmp, at, length);
      r->tmp[length] = '\0';
      log_debug("on_status: %s", r->tmp);
      return 0;
    }

    static int on_body(http_parser* p, const char *at, size_t length)
    {
      auto r = (http_request_closure*) p->data;

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

    static bool32
    do_request(ip_address ip, uint16 port, const char* method,
      const char* host, uintptr host_len,
      const char* path, uintptr path_len,
      http_request_closure* closure)
    {
      socket* conn = closure->request->socket;
      bool32 made_request = false;
      if (conn->is_connected || socket_connect(conn, ip, port))
      {
        astro_assert(conn->ip == ip);
        char hdr[HTTP_MAX_HEADER_SIZE] = {};
        uintptr msg_len = 0;
        msg_len += append_format(hdr, HTTP_MAX_HEADER_SIZE,
          "%s %*s HTTP/1.1\r\n"
          "Host: %*s\r\n"
          "User-Agent: curl/7.37.1\r\n",
          method,
          path_len, path,
          host_len, host);

        msg_len += append_format(hdr, HTTP_MAX_HEADER_SIZE,
          "Connection: Keep-Alive\r\n",
          "Accept: */*\r\n"
          "\r\n");

        log_debug("Sending request (%ld bytes):\n\"%*s\"", msg_len, (int)msg_len - 2, hdr);

        socket_send(conn, (uint8*) hdr, msg_len);
        http_parser parser = {};
        http_parser_init(&parser, HTTP_RESPONSE);
        parser.data = closure;

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

        made_request = true;
      }

      return made_request;
    }
  }

  std::future<http_response>
  http(http_request* req, allocator* alloc)
  {
    return std::async(std::launch::async, [=]()
    {
      const char* url = req->url;
      const char* method = req->method;

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

      http_request_closure closure = {};
      bool32 shared_socket = req->socket != nullptr;
      socket s;
      if (!shared_socket)
      {
        s = socket_create(address_family::inter_network, socket_type::stream, protocol_type::tcp);
        req->socket = &s;
      }

      using namespace std::placeholders;

      closure.request = req;
      closure.response = &res;
      closure.allocator = alloc;

      const char* host = url + seg_host.off;
      uintptr host_len = seg_host.len;
      auto ips = dns::resolve_host_name(host, seg_host.len, address_family::inter_network).get();
      for (auto& ip : ips)
      {
        ip_address_to_string(buffer, sizeof(buffer), &ip);
        log_debug("resolved to %s", buffer);

        const char* path = "/";
        uintptr path_len = 1;
        if (seg_path.len > 0)
        {
          path_len = seg_path.len;
          path = url + seg_path.off;
        }

        if (do_request(ip, port, method, host, host_len, path, path_len, &closure))
        {
          // TODO: Make an option for this functionality.
          uint8 num_redirects = 0;
          http_status_code status = res.status_code;
          while (http_status_code_is_redirect(status))
          {
            auto location = http_header_find_key(res.headers, "Location");
            if (location == nullptr)
              break;

            do_request(ip, port, method, host, host_len,
              location->value, location->value_length, &closure);

            // Check the status code again, in case of chaining redirects.
            status = res.status_code;

            if (num_redirects >= kMaxHttpRedirects)
            {
              log_debug("Max redirects reached. Aborting request.");
              res = {};
              break;
            }
          }

          break;
        }
      }

      if (!shared_socket)
      {
        socket_destroy(req->socket);
        req->socket = nullptr;
      }

      return res;
    });
  }
}}
