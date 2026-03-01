#ifndef WITX_COMPRESS_H
#define WITX_COMPRESS_H

#include <stddef.h>

typedef struct {
  unsigned char *data;
  size_t size;
  int error;
} mem_result;

mem_result decompress_from_memory(const unsigned char *input, size_t input_size);
mem_result compress_from_memory(const unsigned char *input, size_t input_size);

#endif
