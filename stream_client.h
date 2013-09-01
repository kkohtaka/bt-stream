#ifndef __STREAM_CLIENT_H__
#define __STREAM_CLIENT_H__

#include "client.h"
#include "stream.h"

class StreamClient : public Client {
 private:
  Stream &stream_;
  bool running_;
  unsigned int fragment_sequence_;
 public:
  StreamClient(uv_loop_t *loop, Stream &stream);
  ~StreamClient(void);
  void run(void);
};

#endif // __STREAM_CLIENT_H__

