// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_CLIENT_H_
#define SRC_STREAM_CLIENT_H_

#include <uv.h>
#include <inttypes.h>
#include <memory>
#include "./client.h"

class Stream;

class StreamClient : public Client {
 private:
  static const uint8_t http_header[];
  ::uv_timer_t timer_;
  std::shared_ptr<Stream> stream_;
  bool running_;
  uint32_t fragment_sequence_;
  uint32_t age_;
  uint32_t stream_age_;
  uint32_t fragment_offset_;
  uint32_t fragment_length_;
  std::shared_ptr< ::uv_write_t> write_req_;
  static void write_header_static(::uv_timer_t *timer, int status);
  static void write_fragment_static(::uv_timer_t *timer, int status);
  static void close_static(::uv_timer_t *timer, int status);
  void write_header(void);
  void write_fragment(void);
  void on_close(void);

 public:
  StreamClient(::uv_loop_t *loop, std::shared_ptr<Stream> stream);
  virtual ~StreamClient(void);
  void run(void);
};

#endif  // SRC_STREAM_CLIENT_H_

