// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_MOVIE_FRAGMENT_H_
#define SRC_MOVIE_FRAGMENT_H_

#include <string>
#include <memory>

class MovieFragment {
 private:
  int const INITIAL_CLUSTER_LENGTH;
  const int TIMECODE_LAST_OFFSET;
  const int CLUSTER_LENGTH_LAST_OFFSET;
  unsigned char CLUSTER_HEAD[19];
  const unsigned int CLUSTER_HEAD_LENGTH;
  std::shared_ptr<char> data_;
  int data_length_;
  int cluster_offset_;
  int keyframe_offset_;
  int keyframe_length_;

 public:
  MovieFragment(void);
  MovieFragment& operator=(const MovieFragment& movie_fragment);
  ~MovieFragment(void);
  void open_cluster(int time_code);
  void close_cluster(void);
  void append_key_block(
      char *buffer,
      int offset,
      int length,
      int keyframe_offset);
  void append_block(
      char *buffer,
      int offset,
      int length);
  std::shared_ptr<char> data(void);
  int data_length(void);
  int keyframe_offset(void);
  int keyframe_length(void);
};

#endif  // SRC_MOVIE_FRAGMENT_H_
