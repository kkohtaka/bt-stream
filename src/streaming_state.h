// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAMING_STATE_H_
#define SRC_STREAMING_STATE_H_

#include <inttypes.h>
#include <memory>
#include "./stream_input_state.h"
#include "./stream.h"
#include "./stream_input.h"

class StreamingState : public StreamInputState {
 private:
  static const uint32_t ID_CLUSTER = 0x1f43b675;
  static const uint32_t ID_SIMPLEBLOCK = 0xa3;
  static const uint32_t ID_BLOCKGROUP = 0xa0;
  static const uint32_t ID_TIMECODE = 0xe7;
  static const uint32_t MINIMAL_FRAGMENT_LENGTH = 100 * 1024;
  StreamInput *input_;
  std::shared_ptr<Stream> stream_;
  int32_t video_track_number_;
  int32_t cluster_time_code_;
  std::shared_ptr<MovieFragment> fragment_;

 public:
  StreamingState(
      StreamInput *input,
      std::shared_ptr<Stream> stream,
      int32_t video_track_number);
  virtual ~StreamingState(void);
  virtual uint32_t process_data(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
};

#endif  // SRC_STREAMING_STATE_H_
