#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "uv.h"
#include "http-parser/http_parser.h"

class Client {
 protected:
  uv_tcp_t handle_;
  http_parser parser_;
  uv_write_t write_req_;
  void *data_;
 public:
  explicit Client(uv_loop_t *loop);
  virtual ~Client(void);
  int parse_request(
      http_parser_settings *parser_settings,
      ssize_t nread,
      uv_buf_t buf
  );
  int accept(uv_stream_t *server);
  void start_reading(uv_alloc_cb alloc_cb, uv_read_cb read_cb);
  void write(uv_write_t* req, uv_buf_t bufs[], int bufcnt, uv_write_cb write_cb);
  void close(uv_close_cb close_cb);
  void *data(void);
  void set_data(void *data);
};

#endif // __CLIENT_H__

