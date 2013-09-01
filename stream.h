#ifndef __STREAM_H__
#define __STREAM_H__

#include <string>
#include <memory>
#include "movie_fragment.h"

class Stream {
 private:
  int running_;
  std::shared_ptr<unsigned char> header_;
  unsigned int header_length_;
  int fragment_age_;
  std::shared_ptr<MovieFragment> fragment_;
 public:
  Stream(void);
  virtual ~Stream(void);
  bool is_running(void);
  int fragment_age(void);
  std::shared_ptr<MovieFragment> fragment(void);
  std::shared_ptr<unsigned char> header(void);
  unsigned int header_length(void);
  void set_header(std::shared_ptr<unsigned char> header, unsigned int header_length);
  void push_fragment(std::shared_ptr<MovieFragment> fragment);
};

#endif // __STREAM_H__

