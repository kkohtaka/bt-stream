// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./controlled_stream.h"

#include <cstdio>

ControlledStream::ControlledStream(uint32_t max_clients)
  : Stream(),
    MAX_CLIENTS(max_clients),
    num_clients_(0) {
}

ControlledStream::~ControlledStream(void) {
  printf("ControlledStream deleted.\n");
}

bool ControlledStream::subscribe(void) {
  if (num_clients_ < MAX_CLIENTS) {
    ++num_clients_;
    refresh_status();
    return true;
  }
  return false;
}

void ControlledStream::unsubscribe(void) {
  --num_clients_;
  refresh_status();
}

void ControlledStream::refresh_status(void) {
  // [TODO] postEvent
}

