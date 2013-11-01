// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./stream.h"
#include <cstdio>

Stream::Stream(void)
  : running_(true),
    header_(),
    fragment_age_(0),
    fragment_() {
}

Stream::~Stream(void) {
  std::printf("Stream deleted.\n");
}

bool Stream::is_running(void) {
  return running_;
}

uint32_t Stream::fragment_age(void) {
  return fragment_age_;
}

std::shared_ptr<MovieFragment> Stream::fragment(void) {
  return fragment_;
}

std::shared_ptr<uint8_t> Stream::header(void) {
  return header_;
}

uint32_t Stream::header_length(void) {
  return header_length_;
}

void Stream::set_header(
    std::shared_ptr<uint8_t> header,
    uint32_t header_length) {
  header_.swap(header);
  header_length_ = header_length;
}

void Stream::push_fragment(std::shared_ptr<MovieFragment> fragment) {
  if (fragment_age_ == 0) {
    // [TODO] post a server event
  }

  fragment_.swap(fragment);
  ++fragment_age_;

  std::printf("===== FRAGMENT AGE: %d =====\n", fragment_age_);
}

