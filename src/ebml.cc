#include "ebml.h"

#include <iostream>

#include <cstdlib>

EBML::EBML(
    char *buffer,
    unsigned int offset,
    unsigned int length
) :
    id_(0),
    size_(0),
    buffer_(buffer),
    length_(length),
    offset_(offset),
    data_offset_(0) {

#if 0
  std::cout << "=== EBML === ";
  for (unsigned int i = 0; i < 32 && i < length; ++i) {
    std::cout << (int)*(buffer + offset + i) << ' ';
  }
  std::cout << std::endl;

  std::cout << "=== EBML === offset: " << offset << ", length: " << length << std::endl;
#endif

  long size_flag = 0x80;
  long num = 0;
  while (((num |= buffer[offset++] & 0xff) & size_flag) == 0 && size_flag != 0) {
    num <<= 8;
    size_flag <<= 7;
  }
  id_ = num;

  size_flag = 0x80;
  num = 0;
  while (((num |= buffer[offset++] & 0xff) & size_flag) == 0 && size_flag != 0) {
    num <<= 8;
    size_flag <<= 7;
  }
  size_ = num ^ size_flag;

  data_offset_ = offset;

#if DEBUG_PUBLISHER
  std::cout << "==> EBML::id: " << id_ << ", size: " << size_ << ", data_offset: " << data_offset_ << std::endl;
#endif
}

EBML::~EBML(void) {

  //std::cout << "EBML deleted." << std::endl;
}

int EBML::load_unsigned(
    char *buffer,
    unsigned int offset,
    unsigned int length
) {

  long num = 0;
  while (length > 0) {
    --length;
    num <<= 8;
    num |= buffer[offset++] & 0xff;
  }
  return num;
}

int EBML::load_EBML_unsigned(
    char *buffer,
    unsigned int offset,
    unsigned int length
) {

  long size_flag = 0x80;
  long num = 0;
  while (((num |= buffer[offset++] & 0xff) & size_flag) == 0 && size_flag != 0) {
    num <<= 8;
    size_flag <<= 7;
  }
  return num ^ size_flag;
}

int EBML::load_EBML_signed(
    char *buffer,
    unsigned int offset,
    unsigned int length
) {

  long size_flag = 0x80;
  long num = 0;
  long neg_bits = -1 << 7;
  while (((num |= buffer[offset++] & 0xff) & size_flag) == 0 && size_flag != 0) {
    num <<= 8;
    size_flag <<= 7;
    neg_bits <<= 7;
  }
  if ((num & size_flag >> 1) != 0) {
    num |= neg_bits;
  }
  return num;
}

long EBML::id(void) {

  return id_;
}

long EBML::data_size(void) {

  return size_;
}

int EBML::element_size(void) {

  if (size_ == 0x1ffffffffffffffL) {
    return -1;
  }
  if (size_ >= 0x100000000L) {
    // [TODO] Handle the error.
  }
  return (int)(data_offset_ - offset_ + size_);
}

char *EBML::buffer(void) {

  return buffer_;
}

unsigned int EBML::data_offset(void) {

  return data_offset_;
}

unsigned int EBML::element_offset(void) {

  return offset_;
}

int EBML::end_offset(void) {
  if (size_ == 0x1ffffffffffffffL) {
    return -1;
  }
  if ((data_offset_ + size_) >= 0x100000000L) {
    // [TODO] Handle the error.
#if DEBUG_PUBLISHER
    std::cout << "data_offset_: " << data_offset_ << std::endl;
    std::cout << "size_: " << size_ << std::endl;
#endif
    ::exit(EXIT_FAILURE);
  }
  return data_offset_ + size_;
}

