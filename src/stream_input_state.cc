// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./stream_input_state.h"
#include <cstdio>

StreamInputState::StreamInputState(void) {
}

StreamInputState::~StreamInputState(void) {
  std::printf("StreamInputState deleted. %p\n", static_cast<void *>(this));
}

