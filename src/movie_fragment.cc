// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./movie_fragment.h"
#include <cstring>

MovieFragment::MovieFragment(void)
  : INITIAL_CLUSTER_LENGTH(10),
    TIMECODE_LAST_OFFSET(18),
    CLUSTER_LENGTH_LAST_OFFSET(8),
    CLUSTER_HEAD {
        0x1F, 0x43, 0xB6, 0x75,
        0x08, 0x00, 0x00, 0x00,
        0x00, 0xE7, 0x88, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00
    },
    CLUSTER_HEAD_LENGTH(sizeof(CLUSTER_HEAD)),
    data_(new uint8_t[1024 * 1024], [] (uint8_t *p) { delete [] p; }),
    data_length_(0),
    cluster_offset_(-1),
    keyframe_offset_(-1),
    keyframe_length_(-1) {
}

MovieFragment& MovieFragment::operator=(const MovieFragment& movie_fragment) {
  data_ = movie_fragment.data_;
  data_length_ = movie_fragment.data_length_;
  cluster_offset_ = movie_fragment.cluster_offset_;
  keyframe_offset_ = movie_fragment.keyframe_offset_;
  keyframe_length_ = movie_fragment.keyframe_length_;
  return *this;
}

MovieFragment::~MovieFragment(void) {
  std::printf("MovieFragment deleted. %p\n", static_cast<void *>(this));
}

void MovieFragment::open_cluster(int32_t time_code) {
  if (cluster_offset_ != -1) {
    close_cluster();
  }

  memcpy(
      data_.get() + data_length_,
      CLUSTER_HEAD,
      CLUSTER_HEAD_LENGTH);
  cluster_offset_ = data_length_;
  data_length_ += CLUSTER_HEAD_LENGTH;

  int32_t offset = cluster_offset_ + TIMECODE_LAST_OFFSET;
  int32_t code = time_code;
  while (code > 0) {
    data_.get()[offset--] = code;
    code >>= 8;
  }
}

void MovieFragment::close_cluster(void) {
  if (cluster_offset_ == -1) {
    return;
  }

  uint32_t cluster_length =
      data_length_ - cluster_offset_ - INITIAL_CLUSTER_LENGTH;

  uint32_t offset = cluster_offset_ + CLUSTER_LENGTH_LAST_OFFSET;
  uint32_t code = cluster_length;
  while (code > 0) {
    data_.get()[offset--] = code;
    code >>= 8;
  }

  cluster_offset_ = -1;
}

void MovieFragment::append_key_block(
    uint8_t *buffer,
    uint32_t offset,
    uint32_t length,
    uint32_t keyframe_offset
) {
  if (keyframe_offset_ > 0) {
    keyframe_offset_ = data_length_ + (keyframe_offset - offset);
    keyframe_length_ = length - (keyframe_offset - offset);
  }

  append_block(buffer, offset, length);
}

void MovieFragment::append_block(
    uint8_t *buffer,
    uint32_t offset,
    uint32_t length
) {
  if (1024 * 1024 < data_length_ + length) {
    return;
  }

  memcpy(data_.get() + data_length_, buffer + offset, length);
  data_length_ += length;
}

std::shared_ptr<uint8_t> MovieFragment::data(void) {
  return data_;
}

uint32_t MovieFragment::data_length(void) {
  return data_length_;
}

int32_t MovieFragment::keyframe_offset(void) {
  return keyframe_offset_;
}

int32_t MovieFragment::keyframe_length(void) {
  return keyframe_length_;
}

