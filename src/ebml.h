// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_EBML_H_
#define SRC_EBML_H_

#include <inttypes.h>

class EBML {
 private:
  int64_t id_;
  int64_t size_;
  char *buffer_;
  unsigned int length_;
  unsigned int offset_;
  unsigned int data_offset_;

 public:
  EBML(
      char *buffer,
      unsigned int offset,
      unsigned int length);
  ~EBML(void);
  static int load_unsigned(
      char *buffer,
      unsigned int offset,
      unsigned int length);
  static int load_EBML_unsigned(
      char *buffer,
      unsigned int offset,
      unsigned int length);
  static int load_EBML_signed(
      char *buffer,
      unsigned int offset,
      unsigned int length);
  int64_t id(void);
  int64_t data_size(void);
  int element_size(void);
  char *buffer(void);
  unsigned int data_offset(void);
  unsigned int element_offset(void);
  int end_offset(void);
};

#endif  // SRC_EBML_H_

