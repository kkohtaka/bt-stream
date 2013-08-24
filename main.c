#include "uv.h"

#include "cstdio"
#include "cstdlib"
#include "iostream"

static uv_loop_t *loop;

uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size) {

  return uv_buf_init((char *)malloc(suggested_size), suggested_size);
}

void echo_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf) {

  std::cout << "echo_read" << std::endl;

  std::cout << "nread: " << nread << std::endl;
  if (nread < 0) {
    std::cout << "nread < 0" << std::endl;
  } else {
    fwrite(buf.base, nread, 1, stdout);
  }

  if (buf.base) {
    free(buf.base);
  }
}

void on_new_connection(uv_stream_t *server, int status) {

  std::cout << "on_new_connection" << std::endl;

  if (status == -1) {
    return;
  }

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
  } else {
    uv_close((uv_handle_t *)client, NULL);
  }
}

int main() {

  std::cout << "uv_default_loop" << std::endl;
  loop = uv_default_loop();
  uv_tcp_t server;

  std::cout << "uv_tcp_init" << std::endl;
  uv_tcp_init(loop, &server);

  struct sockaddr_in bind_addr = uv_ip4_addr("0.0.0.0", 8080);
  std::cout << "uv_tcp_bind" << std::endl;
  uv_tcp_bind(&server, bind_addr);

  std::cout << "uv_listen" << std::endl;
  int r = uv_listen((uv_stream_t *)&server, 128, on_new_connection);

  if (r) {
    fprintf(stderr, "Listen error\n");
    return EXIT_FAILURE;
  }

  std::cout << "uv_run" << std::endl;
  return uv_run(loop, UV_RUN_DEFAULT);
}

