#include "jvm.h"
#include "jvm_errors.h"

#include <setjmp.h>
#include <stdio.h>

extern jmp_buf exception_handler;

struct jvm_instance jvm_create_instance() {
  return (struct jvm_instance){
    .bytecode_stream=NULL,
    .finished=0,
    .pc=0
  };
}

int jvm_load_program_file(
  struct jvm_instance* instance,
  char* filename
) {
  FILE *filestream = fopen(filename, "r");

  if (filestream == NULL) return 1;
  
  instance->bytecode_stream = filestream;
  instance->finished = 0;
  instance->pc = 0;

  return 0;
}

int jvm_run_program(struct jvm_instance* instance) {
  int exception_caught = setjmp(exception_handler);
  // exceptions can now be thrown using
  // `longjmp(exception_handler, <int>);`

  if (exception_caught) {
    // print stuff or smth else
    printf("exception thrown during jvm execution: %d\n", exception_caught);
    return exception_caught;
  }

  if (instance->bytecode_stream == NULL) {
    longjmp(exception_handler, JVM_UNKNOWN_ERROR);
  }

  bytecode* code;
  while (!instance->finished) {
    // TODO: load chunks at a time for faster runtime
    int res = fread(code, sizeof(bytecode), 1, instance->bytecode_stream);

    if (res) {
      longjmp(exception_handler, JVM_UNKNOWN_ERROR);
    }

    jvm_interpret_bytecode(instance, code);

    instance->pc++;
  }

  return 0;
}

void jvm_interpret_bytecode(struct jvm_instance* instance, bytecode const*  opcode) {
  switch (*opcode) {
    case 0x1:
      break;
  }
}

