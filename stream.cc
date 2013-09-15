#include "stream.h"

#include <iostream>

Stream::Stream(void) :
    running_(true),
    header_(),
    fragment_age_(0),
    fragment_() {
}

Stream::~Stream(void) {

  std::cout << "Stream deleted." << std::endl;
  std::cout.flush();
}

bool Stream::is_running(void) {

  return running_;
}

int Stream::fragment_age(void) {

  return fragment_age_;
}

std::shared_ptr<MovieFragment> Stream::fragment(void) {

  return fragment_;
}

std::shared_ptr<char> Stream::header(void) {

  return header_;
}

unsigned int Stream::header_length(void) {

  return header_length_;
}

void Stream::set_header(std::shared_ptr<char> header, int header_length) {

  header_.swap(header);
  header_length_ = header_length;
}

void Stream::push_fragment(std::shared_ptr<MovieFragment> fragment) {

  if (fragment_age_ == 0) {

    // [TODO] post a server event
  }

  fragment_.swap(fragment);
  ++fragment_age_;

  std::cout << "===== FRAGMENT AGE: " << fragment_age_ << " =====" << std::endl;
}

