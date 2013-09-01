#include "client.h"
#include "stream.h"
#include "stream_input.h"

#include "libuv/include/uv.h"
#include "http-parser/http_parser.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>

static char const * const RESPONSE = \
  "HTTP/1.1 200 OK\r\n" \
  "Content-Type: text/plain\r\n" \
  "Content-Length: 12\r\n" \
  "\r\n" \
  "hello world\n";

static uv_loop_t *loop;
static http_parser_settings parser_settings;

static std::shared_ptr<StreamInput> input;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

void on_close(uv_handle_t *handle) {

  std::cout << "on_close" << std::endl;

  Client *client = static_cast<Client *>(handle->data);

  delete client;
}

uv_buf_t on_alloc(uv_handle_t *handle, size_t suggested_size) {

  std::cout << "on_alloc" << std::endl;
  std::cout << "suggested_size: " << suggested_size << std::endl;
  return uv_buf_init(new char[suggested_size], suggested_size);
}

void on_read(uv_stream_t *tcp, ssize_t nread, uv_buf_t buf) {

  std::cout << "on_read" << std::endl;

  Client *client = static_cast<Client *>(tcp->data);

  std::cout << "nread: " << nread << std::endl;
  if (nread >= 0) {

    int parsed = client->parse_request(
        &parser_settings,
        nread,
        buf
    );
    if (parsed < nread) {
      std::cerr << "parse error" << std::endl;
      client->close(on_close);
    }
  } else {
    if (nread != UV_EOF) {
      std::cerr << uv_strerror(nread) << " error" << std::endl;
    }
  }

  delete [] buf.base;
}

void on_connect(uv_stream_t *server, int status) {

  std::cout << "on_connect" << std::endl;

  if (status == -1) {
    return;
  }

  auto client = std::shared_ptr<Client>(new Client(loop));

  if (client.get()->accept(server) == 0) {
    //client.start_reading(on_alloc, on_read);
    auto stream = std::shared_ptr<Stream>(new Stream());;
    input = std::shared_ptr<StreamInput>(new StreamInput(stream, client));
    input.get()->run();
  } else {
    client.get()->close(on_close);
  }
}

void on_write(uv_write_t *req, int status) {

  std::cout << "on_write" << std::endl;

  uv_close(reinterpret_cast<uv_handle_t *>(req->handle), on_close);

  write_req_t *wr = reinterpret_cast<write_req_t *>(req);
  delete wr->buf.base;
  delete wr;
}

int on_headers_complete(http_parser *parser) {

  std::cout << "on_headers_complete" << std::endl;

  Client *client = static_cast<Client *>(parser->data);

  ssize_t len = strlen(RESPONSE);
  write_req_t *wr = new write_req_t;
  wr->buf = uv_buf_init(new char[len], len);
  strncpy(wr->buf.base, RESPONSE, len);

  client->write(&wr->req, &wr->buf, 1, on_write);

  return 1;
}

int main() {

  unsigned int port = 8080;

  parser_settings.on_headers_complete = on_headers_complete;

  loop = uv_default_loop();

  uv_tcp_t server;
  int r = uv_tcp_init(loop, &server);
  if (r != 0) {
    std::cerr << uv_strerror(r) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  struct sockaddr_in bind_addr = uv_ip4_addr("0.0.0.0", port);
  r = uv_tcp_bind(&server, bind_addr);
  if (r != 0) {
    std::cerr << uv_strerror(r) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  r = uv_listen((uv_stream_t *)&server, 128, on_connect);
  if (r != 0) {
    std::cerr << uv_strerror(r) << " error" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "listening on port: " << port << std::endl;
  return uv_run(loop, UV_RUN_DEFAULT);
}

