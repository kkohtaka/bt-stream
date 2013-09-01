#ifndef __EBML_H__
#define __EBML_H__

class EBML {
 private:
  int id_;
  int size_;
  char *buffer_;
  unsigned int length_;
  unsigned int offset_;
  unsigned int data_offset_;
 public:
  EBML(
      char *buffer,
      unsigned int length,
      unsigned int offset
  );
  ~EBML(void);
  static int load_unsigned(
      char *buffer,
      unsigned int length,
      unsigned int offset
  );
  static int load_EBML_unsigned(
      char *buffer,
      unsigned int length,
      unsigned int offset
  );
  static int load_EBML_signed(
      char *buffer,
      unsigned int length,
      unsigned int offset
  );
  int id(void);
  unsigned int data_size(void);
  int element_size(void);
  char *buffer(void);
  unsigned int data_offset(void);
  unsigned int element_offset(void);
  int end_offset(void);
};

#endif // __EBML_H__

