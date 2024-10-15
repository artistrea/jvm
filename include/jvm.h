#ifndef JVM_H
#define JVM_H

#include <inttypes.h>
#include <string.h> // size_t

typedef uint8_t bytecode;

struct jvm_instance {
  size_t n_bytecodes;
  bytecode* bytecodes;
  int finished;
  size_t pc;
};

struct jvm_instance jvm_create_instance();

int jvm_load_program(
  struct jvm_instance* instance,
  size_t n_bytecodes,
  bytecode* bytecodes
);

int jvm_run_program(struct jvm_instance* instance);

void jvm_interpret_bytecode(struct jvm_instance* instance, bytecode const* code);

#endif // JVM_H

