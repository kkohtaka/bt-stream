#include "libuv/include/uv.h"
#include "http-parser/http_parser.h"

#include "cstdio"
#include "cstdlib"
#include "cstring"
#include "iostream"

static char const * const RESPONSE = \
  "HTTP/1.1 200 OK\r\n" \
  "Content-Type: text/plain\r\n" \
  "Content-Length: 12\r\n" \
  "\r\n" \
  "hello world\n";

static uv_loop_t *loop;
static int request_num = 0;
static http_parser_settings parser_settings;

typedef struct {
  uv_tcp_t handle;
  http_parser parser;
  uv_write_t write_req;
  int request_num;
} client_t;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

void on_close(uv_handle_t *handle) {

  std::cout << "on_close" << std::endl;

  client_t *client = static_cast<client_t *>(handle->data);

  delete client;
}

uv_buf_t on_alloc(uv_handle_t *handle, size_t suggested_size) {

  std::cout << "on_alloc" << std::endl;

  return uv_buf_init(new char[suggested_size], suggested_size);
}

void on_read(uv_stream_t *tcp, ssize_t nread, uv_buf_t buf) {

  std::cout << "on_read" << std::endl;

  client_t *client = static_cast<client_t *>(tcp->data);

  std::cout << "nread: " << nread << std::endl;
  if (nread >= 0) {

    int parsed = http_parser_execute(
        &client->parser,
        &parser_settings,
        buf.base,
        nread
    );
    if (parsed < nread) {
      std::cerr << "parse error" << std::endl;
      uv_close(reinterpret_cast<uv_handle_t *>(&client->handle), on_close);
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

  request_num += 1;
  client_t *client = new client_t;
  client->request_num = request_num;

  uv_tcp_init(loop, &client->handle);
  http_parser_init(&client->parser, HTTP_REQUEST);

  client->parser.data = client;
  client->handle.data = client;

  if (uv_accept(server, reinterpret_cast<uv_stream_t *>(&client->handle)) == 0) {
    uv_read_start(reinterpret_cast<uv_stream_t *>(&client->handle), on_alloc, on_read);
  } else {
    uv_close(reinterpret_cast<uv_handle_t *>(&client->handle), on_close);
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

  client_t *client = static_cast<client_t *>(parser->data);

  ssize_t len = strlen(RESPONSE);
  write_req_t *wr = new write_req_t;
  wr->buf = uv_buf_init(new char[len], len);
  strncpy(wr->buf.base, RESPONSE, len);

  uv_write(
      &wr->req,
      reinterpret_cast<uv_stream_t *>(&client->handle),
      &wr->buf,
      1,
      on_write
  );

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

