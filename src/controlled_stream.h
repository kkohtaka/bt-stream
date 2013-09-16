// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_CONTROLLED_STREAM_H_
#define SRC_CONTROLLED_STREAM_H_

#include <string>
#include "./stream.h"

class ControlledStream : public Stream {
 private:
  const unsigned int MAX_CLIENTS;
  unsigned int num_clients_;
 public:
  explicit ControlledStream(unsigned int num_clients);
  virtual ~ControlledStream(void);
  bool subscribe(void);
  void unsubscribe(void);
  void refresh_status(void);
};

#endif  // SRC_CONTROLLED_STREAM_H_

