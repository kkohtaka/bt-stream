// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include <uv.h>
#include <http_parser.h>
#include <memory>
#include "./Util.h"
#include "./HCI.h"
#include "./SDP.h"
#include "./RFCOMM.h"
#include "./client.h"
#include "./stream.h"
#include "./stream_input.h"
#include "./stream_client.h"

static ::uv_loop_t *loop;
static std::shared_ptr<Stream> stream;
static std::shared_ptr<StreamInput> input;
static std::shared_ptr<StreamClient> stream_client;

static const bdaddr_t
  bdaddr_any({{ 0, 0, 0, 0, 0, 0 }}),
  bdaddr_local({{ 0, 0, 0, 0xff, 0xff, 0xff }});

static const uint8_t SERVICE_UUID[] = {
  0x14, 0x7d, 0xe4, 0xe0,
  0x40, 0xb5, 0x11, 0xe3,
  0xb6, 0xa2, 0x00, 0x02,
  0xa5, 0xd5, 0xc5, 0x1b
};

static const uint8_t SERVICE_PORT = 2;

int start_publisher_server(::uv_tcp_t *server, const uint32_t port) {
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

#if 0
int start_consumer_server(::uv_tcp_t *server, const uint32_t port) {
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
#endif

int start_consumer_server(
    ::uv_rfcomm_t *server,
    ::bdaddr_t bdaddr,
    const uint8_t channel) {
  int ret = ::uv_rfcomm_init(loop, server);
  if (ret != 0) {
    std::fprintf(stderr, "%s error\n", ::uv_strerror(::uv_last_error(loop)));
    return ret;
  }

  struct ::sockaddr_rc bind_addr = { 0 };

  bind_addr.rc_family = AF_BLUETOOTH;
  bind_addr.rc_bdaddr = bdaddr;
  bind_addr.rc_channel = channel;

  ret = ::uv_rfcomm_bind(server, bind_addr);
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

  std::printf("===== CONSUMER SERVER LISTENING ON PORT: %d =====\n", channel);
  return 0;
}

int main() {
  int ret;

  loop = ::uv_default_loop();

  stream = std::shared_ptr<Stream>(new Stream());

  ::uv_tcp_t publisher_server;
  const uint32_t publisher_port = 8081;
  ret = start_publisher_server(&publisher_server, publisher_port);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

#if 0
  ::uv_tcp_t consumer_server;
  const uint32_t consumer_port = 8080;
  ret = start_consumer_server(&consumer_server, consumer_port);
  if (ret != 0) {
    return EXIT_FAILURE;
  }
#else
  ::uv_rfcomm_t consumer_server;

  HCI::LocalDevice
    local_device = HCI::LocalDevice();

  std::printf(
      "Local Device Name: %s\n",
      local_device.get_name().c_str());
  std::printf(
      "Local Device Address: %s\n",
      Util::get_bdaddr_str(local_device.get_address()).c_str());

  ret = start_consumer_server(
      &consumer_server,
      local_device.get_address(),
      SERVICE_PORT);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

  SDP::Session
    session(&bdaddr_any, &bdaddr_local);
  session.register_service(
    SERVICE_UUID,
    SERVICE_PORT,
    "RFCOMM Server",
    "Kazumasa Kohtaka <kkohtaka@gmail.com>",
    "A sample implementation of RFCOMM server");
#endif

  return ::uv_run(loop, UV_RUN_DEFAULT);
}

