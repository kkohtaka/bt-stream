#include "ebml.h"

EBML::EBML(
    char *buffer,
    unsigned int length,
    unsigned int offset
) :
    buffer_(buffer),
    length_(length),
    offset_(offset) {

  int size_flag = 0x80;
  int num = 0;
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
}

EBML::~EBML(void) {
}

int EBML::load_unsigned(
    char *buffer,
    unsigned int length,
    unsigned int offset
) {

  int num = 0;
  while (length > 0) {
    --length;
    num <<= 8;
    num |= buffer[offset++] & 0xff;
  }
  return num;
}

int EBML::load_EBML_unsigned(
    char *buffer,
    unsigned int length,
    unsigned int offset
) {

  int size_flag = 0x80;
  int num = 0;
  while (((num |= buffer[offset++] & 0xff) & size_flag) == 0 && size_flag != 0) {
    num <<= 8;
    size_flag <<= 7;
  }
  return num ^ size_flag;
}

int EBML::load_EBML_signed(
    char *buffer,
    unsigned int length,
    unsigned int offset
) {

  int size_flag = 0x80;
  int num = 0;
  int neg_bits = -1 << 7;
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

int EBML::id(void) {

  return id_;
}

unsigned int EBML::data_size(void) {

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
  }
  return data_offset_ + size_;
}

