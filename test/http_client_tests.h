/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/net/http_client.h>

using namespace astro;
using namespace astro::net;

using namespace std;

using test_http_req = std::tuple<const char*, http_status_code>;

TEST http_get() {

  test_http_req test_data[] = {
    test_http_req("http://mattrudder.com", http_status_code::OK),
  };

  for (auto&& test : test_data)
  {
    const char* test_host;
    http_status_code expected_status_code;
    std::tie(test_host, expected_status_code) = test;

    http_response res = http_request(test_host).get();
    ASSERT_EQ(expected_status_code, res.status_code);
  }

  PASS();
}

SUITE(http_client_tests) { RUN_TEST(http_get); }
