#ifndef JVM_UTILS_H
#define JVM_UTILS_H

#include "jvm.h"

#include <inttypes.h>

int jvm_utils_read_bytecodes_from_file(
  char* filename,
  size_t n_bytecodes,
  bytecode* bytecodes
);

#endif // JVM_UTILS_H


