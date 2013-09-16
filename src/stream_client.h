#ifndef __STREAM_CLIENT_H__
#define __STREAM_CLIENT_H__

#include "client.h"
#include "uv.h"

#include <memory>

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

#endif // __STREAM_CLIENT_H__

