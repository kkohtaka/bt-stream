// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include <memory>

#include "./client.h"
#include "./stream.h"
#include "./stream_input.h"
#include "./stream_client.h"

#include "libuv/include/uv.h"
#include "http-parser/http_parser.h"

static ::uv_loop_t *loop;
static std::shared_ptr<Stream> stream;
static std::shared_ptr<StreamInput> input;
static std::shared_ptr<StreamClient> stream_client;

int start_publisher_server(::uv_tcp_t *server, const unsigned int port) {
  int ret = ::uv_tcp_init(loop, server);
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  struct ::sockaddr_in bind_addr = ::uv_ip4_addr("0.0.0.0", port);
  ret = ::uv_tcp_bind(server, bind_addr);
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  ret = ::uv_listen(
      (::uv_stream_t *)server,
      128,
      [] (::uv_stream_t *server, int status) {
        std::printf("on_connect\n");

        if (status == -1) {
          return;
        }

        auto client = std::shared_ptr<Client>(new Client(loop));

        if (client.get()->accept(server) == 0) {
          input = std::shared_ptr<StreamInput>(new StreamInput(stream, client));
          input.get()->run();
        } else {
          client.get()->close([] (::uv_handle_t *handle) {});
        }
      });
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  std::printf("===== PUBLISHER SERVER LISTENING ON PORT: %d =====\n", port);
  return 0;
}

int start_consumer_server(::uv_tcp_t *server, const unsigned int port) {
  int ret = ::uv_tcp_init(loop, server);
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  struct ::sockaddr_in bind_addr = ::uv_ip4_addr("0.0.0.0", port);
  ret = ::uv_tcp_bind(server, bind_addr);
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  ret = ::uv_listen(
      (::uv_stream_t *)server,
      128,
      [] (::uv_stream_t *server, int status) {
        std::printf("on_connect\n");

        if (status == -1) {
          return;
        }

        stream_client =
            std::shared_ptr<StreamClient>(new StreamClient(loop, stream));

        if (stream_client.get()->accept(server) == 0) {
          stream_client.get()->run();
        } else {
          stream_client.get()->close([] (::uv_handle_t *handle) {});
        }
      });
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  std::printf("===== CONSUMER SERVER LISTENING ON PORT: %d =====\n", port);
  return 0;
}

int main() {
  int ret;

  loop = ::uv_default_loop();

  stream = std::shared_ptr<Stream>(new Stream());

  ::uv_tcp_t publisher_server;
  const unsigned int publisher_port = 8080;
  ret = start_publisher_server(&publisher_server, publisher_port);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

  ::uv_tcp_t consumer_server;
  const unsigned int consumer_port = 8081;
  ret = start_consumer_server(&consumer_server, consumer_port);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

  return ::uv_run(loop, UV_RUN_DEFAULT);
}

