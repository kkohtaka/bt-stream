// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_EBML_H_
#define SRC_EBML_H_

#include <inttypes.h>

class EBML {
 private:
  int64_t id_;
  int64_t size_;
  uint8_t *buffer_;
  uint32_t length_;
  uint32_t offset_;
  uint32_t data_offset_;

 public:
  EBML(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
  ~EBML(void);
  static uint32_t load_unsigned(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
  static uint32_t load_EBML_unsigned(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
  static int32_t load_EBML_signed(
      uint8_t *buffer,
      uint32_t offset,
      uint32_t length);
  int64_t id(void);
  int64_t data_size(void);
  int32_t element_size(void);
  uint8_t *buffer(void);
  uint32_t data_offset(void);
  uint32_t element_offset(void);
  int32_t end_offset(void);
};

#endif  // SRC_EBML_H_

