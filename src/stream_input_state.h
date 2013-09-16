// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAM_INPUT_STATE_H_
#define SRC_STREAM_INPUT_STATE_H_

class StreamInputState {
 public:
  StreamInputState(void);
  virtual ~StreamInputState(void);
  virtual int process_data(
      char *buffer,
      unsigned int offset,
      unsigned int length) = 0;
};

#endif  // SRC_STREAM_INPUT_STATE_H_

