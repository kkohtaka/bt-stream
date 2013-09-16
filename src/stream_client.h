// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_CLIENT_H_
#define SRC_STREAM_CLIENT_H_

#include <memory>

#include "./client.h"
#include "libuv/include/uv.h"

class Stream;

class StreamClient : public Client {
 private:
  static const char http_header[];
  ::uv_timer_t timer_;
  std::shared_ptr<Stream> stream_;
  bool running_;
  unsigned int fragment_sequence_;
  int age_;
  unsigned int stream_age_;
  unsigned int fragment_offset_;
  unsigned int fragment_length_;
  ::uv_write_t *write_req_;
  static void write_header_static(::uv_timer_t *timer, int status);
  static void write_fragment_static(::uv_timer_t *timer, int status);
  static void close_static(::uv_timer_t *timer, int status);
  void write_header(void);
  void write_fragment(void);
  void on_close(void);
 public:
  StreamClient(uv_loop_t *loop, std::shared_ptr<Stream> stream);
  ~StreamClient(void);
  void run(void);
};

#endif  // SRC_STREAM_CLIENT_H_

