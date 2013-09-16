#ifndef __HEADER_DETECTION_STATE_H__
#define __HEADER_DETECTION_STATE_H__

#include "stream_input_state.h"
#include "stream.h"
#include "stream_input.h"
#include <memory>

class HeaderDetectionState : public StreamInputState {
 private:
  static const int ID_EBML = 0x1A45DFA3;
  static const int ID_SEGMENT = 0x18538067;
  static const int ID_INFO = 0x1549A966;
  static const int ID_TRACKS = 0x1654AE6B;
  static const int ID_TRACKTYPE = 0x83;
  static const int ID_TRACKNUMBER = 0xD7;
  static const int TRACK_TYPE_VIDEO = 1;
  static const unsigned char infiniteSegment[];
  std::shared_ptr<StreamInput> input_;
  std::shared_ptr<Stream> stream_;
 public:
  HeaderDetectionState(
      std::shared_ptr<StreamInput> input,
      std::shared_ptr<Stream> stream
  );
  virtual ~HeaderDetectionState(void);
  virtual int process_data(
      char *buffer,
      unsigned int offset,
      unsigned int length
  );
};

#endif // __HEADER_DETECTION_STATE_H__
