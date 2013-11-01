// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include <uv.h>
#include <http_parser.h>

class Client {
 private:
  ::uv_tcp_t handle_;
  ::http_parser parser_;
  ::uv_write_t write_req_;
  void *data_;

 public:
  explicit Client(::uv_loop_t *loop);
  virtual ~Client(void);
  size_t parse_request(
      ssize_t nread,
      ::uv_buf_t buf,
      ::http_data_cb on_body);
  int accept(uv_stream_t *server);
  void start_reading(
      ::uv_alloc_cb alloc_cb,
      ::uv_read_cb read_cb);
  void stop_reading(void);
  void write(
      ::uv_write_t* req,
      ::uv_buf_t bufs[],
      int bufcnt,
      ::uv_write_cb write_cb);
  void close(::uv_close_cb close_cb);
  void *data(void);
  void set_data(void *data);
};

#endif  // SRC_CLIENT_H_

