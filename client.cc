#include "client.h"

#include <iostream>

Client::Client(uv_loop_t *loop) :
    handle_(),
    parser_(),
    write_req_(),
    data_(nullptr) {

  ;;uv_tcp_init(loop, &handle_);
  ::http_parser_init(&parser_, HTTP_REQUEST);

  handle_.data = this;
  parser_.data = this;
}

Client::~Client(void) {
}

int Client::parse_request(
    ssize_t nread,
    uv_buf_t buf,
    http_data_cb on_body
) {

  static http_parser_settings settings;
  settings.on_body = on_body;
  return http_parser_execute(
    &parser_,
    &settings,
    buf.base,
    nread
  );
}

int Client::accept(uv_stream_t *server) {

  return uv_accept(server, reinterpret_cast<uv_stream_t *>(&handle_));
}

void Client::start_reading(uv_alloc_cb alloc_cb, uv_read_cb read_cb) {

  uv_read_start(reinterpret_cast<uv_stream_t *>(&handle_), alloc_cb, read_cb);
}

void Client::write(uv_write_t *req, uv_buf_t bufs[], int bufcnt, uv_write_cb write_cb) {

  uv_write(req, reinterpret_cast<uv_stream_t *>(&handle_), bufs, bufcnt, write_cb);
}

void Client::close(uv_close_cb close_cb) {

  uv_close(reinterpret_cast<uv_handle_t *>(&handle_), close_cb);
}

void *Client::data(void) {

  return data_;
}

void Client::set_data(void *data) {

  data_ = data;
}

