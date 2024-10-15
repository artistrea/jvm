#include "jvm.h"

struct jvm_instance jvm_create_instance() {
  return (struct jvm_instance){
    .bytecodes=NULL,
    .n_bytecodes=0,
    .finished=0,
    .pc=0
  };
}

int jvm_load_program(
  struct jvm_instance* instance,
  size_t n_bytecodes,
  bytecode* bytecodes
) {
  instance->bytecodes = bytecodes;
  instance->n_bytecodes = n_bytecodes;
  instance->finished = 0;
  instance->pc = 0;

  return 0;
}

int jvm_run_program(struct jvm_instance* instance) {
  while (!instance->finished) {
    jvm_interpret_bytecode(instance, &instance->bytecodes[instance->pc]);
    instance->pc++;
  }

  return 0;
}

void jvm_interpret_bytecode(struct jvm_instance* instance, bytecode const*  opcode) {
  switch (*opcode) {
    case 1:
      break;
  }
}

