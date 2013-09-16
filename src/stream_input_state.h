// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_INPUT_STATE_H_
#define SRC_STREAM_INPUT_STATE_H_

#include <inttypes.h>

class StreamInputState {
 public:
  StreamInputState(void);
  virtual ~StreamInputState(void);
  virtual uint32_t process_data(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length) = 0;
};

#endif  // SRC_STREAM_INPUT_STATE_H_

