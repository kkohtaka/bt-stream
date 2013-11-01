// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_HEADER_DETECTION_STATE_H_
#define SRC_HEADER_DETECTION_STATE_H_

#include <inttypes.h>
#include <memory>
#include "./stream_input_state.h"
#include "./stream.h"
#include "./stream_input.h"

class HeaderDetectionState : public StreamInputState {
 private:
  static const int32_t ID_EBML = 0x1A45DFA3;
  static const int32_t ID_SEGMENT = 0x18538067;
  static const int32_t ID_INFO = 0x1549A966;
  static const int32_t ID_TRACKS = 0x1654AE6B;
  static const int32_t ID_TRACKTYPE = 0x83;
  static const int32_t ID_TRACKNUMBER = 0xD7;
  static const int32_t TRACK_TYPE_VIDEO = 1;
  static const uint8_t infiniteSegment[];
  StreamInput *input_;
  std::shared_ptr<Stream> stream_;
 public:
  HeaderDetectionState(
      StreamInput *input,
      std::shared_ptr<Stream> stream);
  virtual ~HeaderDetectionState(void);
  virtual uint32_t process_data(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
};

#endif  // SRC_HEADER_DETECTION_STATE_H_
