// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_STREAMING_STATE_H_
#define SRC_STREAMING_STATE_H_

#include <memory>

#include "./stream_input_state.h"
#include "./stream.h"
#include "./stream_input.h"

class StreamingState : public StreamInputState {
 private:
  static const int ID_CLUSTER = 0x1f43b675;
  static const int ID_SIMPLEBLOCK = 0xa3;
  static const int ID_BLOCKGROUP = 0xa0;
  static const int ID_TIMECODE = 0xe7;
  static const int MINIMAL_FRAGMENT_LENGTH = 100 * 1024;
  std::shared_ptr<StreamInput> input_;
  std::shared_ptr<Stream> stream_;
  int video_track_number_;
  int cluster_time_code_;
  std::shared_ptr<MovieFragment> fragment_;

 public:
  StreamingState(
      std::shared_ptr<StreamInput> input,
      std::shared_ptr<Stream> stream,
      int video_track_number);
  ~StreamingState(void);
  virtual int process_data(
      char *buffer,
      unsigned int offset,
      unsigned int length);
};

#endif  // SRC_STREAMING_STATE_H_
