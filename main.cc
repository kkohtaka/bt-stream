#include "client.h"
#include "stream.h"
#include "stream_input.h"

#include "libuv/include/uv.h"
#include "http-parser/http_parser.h"

#include <iostream>
#include <memory>

static ::uv_loop_t *loop;
static std::shared_ptr<StreamInput> input;

int main() {

  unsigned int port = 8080;

  loop = ::uv_default_loop();

  ::uv_tcp_t server;
  int ret = ::uv_tcp_init(loop, &server);
  if (ret != 0) {
    std::cerr << ::uv_strerror(ret) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  struct ::sockaddr_in bind_addr = ::uv_ip4_addr("0.0.0.0", port);
  ret = ::uv_tcp_bind(&server, bind_addr);
  if (ret != 0) {
    std::cerr << ::uv_strerror(ret) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  ret = ::uv_listen(
      (::uv_stream_t *)&server,
      128,
      [] (::uv_stream_t *server, int status) {

        std::cout << "on_connect" << std::endl;

        if (status == -1) {
          return;
        }

        auto client = std::shared_ptr<Client>(new Client(loop));

        if (client.get()->accept(server) == 0) {
          auto stream = std::shared_ptr<Stream>(new Stream());;
          input = std::shared_ptr<StreamInput>(new StreamInput(stream, client));
          input.get()->run();
        } else {
          client.get()->close([] (::uv_handle_t *handle) {});
        }
      }
  );
  if (ret != 0) {
    std::cerr << ::uv_strerror(ret) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "===== LISTENING ON PORT: " << port << " =====" << std::endl;
  return ::uv_run(loop, UV_RUN_DEFAULT);
}

