// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_MOVIE_FRAGMENT_H_
#define SRC_MOVIE_FRAGMENT_H_

#include <inttypes.h>
#include <string>
#include <memory>

class MovieFragment {
 private:
  uint32_t const INITIAL_CLUSTER_LENGTH;
  const uint32_t TIMECODE_LAST_OFFSET;
  const uint32_t CLUSTER_LENGTH_LAST_OFFSET;
  unsigned char CLUSTER_HEAD[19];
  const uint32_t CLUSTER_HEAD_LENGTH;
  std::shared_ptr<uint8_t> data_;
  uint32_t data_length_;
  int32_t cluster_offset_;
  int32_t keyframe_offset_;
  int32_t keyframe_length_;

 public:
  MovieFragment(void);
  MovieFragment& operator=(const MovieFragment& movie_fragment);
  ~MovieFragment(void);
  void open_cluster(int32_t time_code);
  void close_cluster(void);
  void append_key_block(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length,
      uint32_t keyframe_offset);
  void append_block(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
  std::shared_ptr<uint8_t> data(void);
  uint32_t data_length(void);
  int32_t keyframe_offset(void);
  int32_t keyframe_length(void);
};

#endif  // SRC_MOVIE_FRAGMENT_H_

