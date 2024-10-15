#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jvm.h"
#include "jvm_utils.h"

int main(
  int argc,
  char **argv
) {
  printf("opa\n");

  struct jvm_instance instance = jvm_create_instance();

  size_t n_bytecodes;
  bytecode* bytecodes;

  // strlen("main.c") == 6
  char* file = (char*)malloc(strlen(argv[0]) - 6);

  int res = jvm_utils_read_bytecodes_from_file(
    "./test.hex",
    n_bytecodes,
    bytecodes
  );

  if (res) return res;

  res = jvm_load_program(
    &instance,
    n_bytecodes,
    bytecodes
  );

  if (res) return res;

  res = jvm_run_program(&instance);

  if (res) return res;

  return 0;
}
