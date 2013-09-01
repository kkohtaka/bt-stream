#ifndef __CONTROLLED_STREAM_H__
#define __CONTROLLED_STREAM_H__

#include <string>
#include "stream.h"

class ControlledStream : public Stream {
 private:
  const unsigned int MAX_CLIENTS;
  unsigned int num_clients_;
 public:
  ControlledStream(unsigned int num_clients);
  virtual ~ControlledStream(void);
  bool subscribe(void);
  void unsubscribe(void);
  void refresh_status(void);
};

#endif // __CONTROLLED_STREAM_H__

