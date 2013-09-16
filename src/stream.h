// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_H_
#define SRC_STREAM_H_

#include <string>
#include <memory>

#include "./movie_fragment.h"

class Stream {
 private:
  int running_;
  std::shared_ptr<char> header_;
  unsigned int header_length_;
  unsigned int fragment_age_;
  std::shared_ptr<MovieFragment> fragment_;
 public:
  Stream(void);
  virtual ~Stream(void);
  bool is_running(void);
  int fragment_age(void);
  std::shared_ptr<MovieFragment> fragment(void);
  std::shared_ptr<char> header(void);
  unsigned int header_length(void);
  void set_header(std::shared_ptr<char> header, int header_length);
  void push_fragment(std::shared_ptr<MovieFragment> fragment);
};

#endif  // SRC_STREAM_H_

