#include "controlled_stream.h"

#include <iostream>

ControlledStream::ControlledStream(unsigned int max_clients) :
    Stream(),
    MAX_CLIENTS(max_clients),
    num_clients_(0) {
}

ControlledStream::~ControlledStream(void) {

  std::cout << "ControlledStream deleted." << std::endl;
}

bool ControlledStream::subscribe(void) {

  if (num_clients_ < MAX_CLIENTS) {
    ++num_clients_;
    refresh_status();
    return true;
  }
  return false;
}

void ControlledStream::unsubscribe(void) {

  --num_clients_;
  refresh_status();
}

void ControlledStream::refresh_status(void) {

  // [TODO] postEvent
}

