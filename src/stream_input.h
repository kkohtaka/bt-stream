// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_INPUT_H_
#define SRC_STREAM_INPUT_H_

#include <memory>

#include <ctime>

#include "./stream.h"
#include "./client.h"
#include "./stream_input_state.h"

class StreamInputBuffer {
 public:
  std::shared_ptr<char> buffer;
  size_t buffer_length;
  size_t offset;
  size_t length;
  StreamInputBuffer(void) :
      buffer(new char[65536], [] (char *p) { delete [] p; }),
      buffer_length(65536),
      offset(0),
      length(0) {
  };
  ~StreamInputBuffer(void) {
  };
};

class StreamInput {
 private:
  std::shared_ptr<Stream> stream_;
  std::shared_ptr<Client> client_;
  bool running_;
  bool header_parsed_;
  unsigned char *header_;
  unsigned int header_length_;
  int chunk_size_;
  std::shared_ptr<StreamInputState> state_;
  StreamInputBuffer buffer_;
  uv_buf_t on_alloc(size_t suggested_size);
  void on_read(ssize_t nread, uv_buf_t buf);
  void on_close(void);
  int on_body(const char *buf, size_t len);

 public:
  StreamInput(
      std::shared_ptr<Stream> stream,
      std::shared_ptr<Client> client);
  ~StreamInput(void);
  bool is_running(void);
  unsigned char *header(void);
  unsigned int header_length(void);
  void set_header(unsigned char *header, unsigned int header_length);
  void run(void);
  void change_state(std::shared_ptr<StreamInputState> state);
};

#endif  // SRC_STREAM_INPUT_H_

