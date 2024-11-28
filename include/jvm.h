#ifndef JVM_H
#define JVM_H

// to consider: big endian when operator uses more than 1 byte
// e.g. 16bit var = ((1st byte) << 8) | (2nd byte)

// there are 2 types of values: reference and primitive values

// byte: -128 to 127
// short: -32768 to 32767
// int: -2147483648 to -2147483647
// long: -9223372036854775808 to -9223372036854775807
// char: '\u0000' to '\uffff', or, 0 to 65535

// no need for concurrency impl
// no need for garbage collector impl

#include <inttypes.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> // size_t

typedef uint8_t bytecode;

struct jvm_instance {
  bytecode* bytecodes;
  int finished;
  size_t pc;
};

struct jvm_instance jvm_create_instance();

extern jmp_buf exception_handler;

int jvm_load_program_file(
  struct jvm_instance* instance,
  char* filename
);

int jvm_run_program(struct jvm_instance* instance);

void jvm_interpret_bytecode(struct jvm_instance* instance, bytecode const* code);

#endif // JVM_H

