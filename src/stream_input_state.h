#ifndef __STREAM_INPUT_STATE_H__
#define __STREAM_INPUT_STATE_H__

class StreamInputState {
 public:
  StreamInputState(void);
  virtual ~StreamInputState(void);
  virtual int process_data(
      char *buffer,
      unsigned int offset,
      unsigned int length
  ) = 0;
};

#endif // __STREAM_INPUT_STATE_H__

