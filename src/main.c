#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;

// default value to most of these is 0
typedef int8_t java_byte;
typedef int16_t java_short;
typedef int32_t java_int;
typedef int64_t java_long;

typedef uint16_t java_char; // default is the null code pointer '\u0000' (?)

typedef float java_float; // default is +0.0f
typedef double java_double; // default is +0.0

union jvm_variable {
  java_byte j_byte;
  java_short j_short;
  java_int j_int;
  java_char j_char;
  java_float j_float;
  // by joining two variables:
  // java_double j_double;
  // java_long j_long;
};

enum {
  TAG__CONSTANT_Class = 7,
  TAG__CONSTANT_Fieldref = 9,
  TAG__CONSTANT_MethodRef = 10,
  TAG__CONSTANT_InterfaceMethodRef = 11,
  TAG__CONSTANT_String = 8,
  TAG__CONSTANT_Integer = 3,
  TAG__CONSTANT_Float = 4,
  TAG__CONSTANT_Long = 5,
  TAG__CONSTANT_Double = 6,
  TAG__CONSTANT_NameAndType = 12,
  TAG__CONSTANT_Utf8 = 1,
  TAG__CONSTANT_MethodHandle = 15,
  TAG__CONSTANT_MethodType = 16,
  TAG__CONSTANT_InvokeDynamic = 18,
};

struct cp_info {
  u1 tag;
  union {
    struct {
        u2 name_index;
    } Class;
    struct {
      u2 class_index;
      u2 name_and_type_index;
    } Fieldref;
    struct {
      u2 class_index;
      u2 name_and_type_index;
    } Methodref;
    struct {
      u2 class_index;
      u2 name_and_type_index;
    } InterfaceMethodref;
    struct {
      u2 name_index;
      u2 descriptor_index;
    } NameAndType;
    struct {
      u2 length;
      u1 *bytes;
    } Utf8;
    struct {
      u2 string_index;
    } String;
    struct {
      u4 bytes; // big endian (same as C)
    } Integer;
    struct {
      u4 bytes;
    } Float;
    struct {
      u4 high_bytes;
      u4 low_bytes;
    } Long;
    struct {
      u4 high_bytes;
      u4 low_bytes;
    } Double;
  };
};

struct attribute_info {
  u2 attribute_name_index;
  u4 attribute_length;
  // TODO: use union here for different attributes
  u1 *info;
};


struct field_info {
  u2 access_flags;
  u2 name_index;
  u2 descriptor_index;

  u2 attributes_count;
  struct attribute_info *attributes;
};

struct method_info {
  u2 access_flags;
  u2 name_index;
  u2 descriptor_index;

  u2 attributes_count;
  struct attribute_info *attributes;
};

enum {
  ACCESS_FLAGS__ACC_PUBLIC     = 0x0001,
  ACCESS_FLAGS__ACC_FINAL      = 0x0010,
  ACCESS_FLAGS__ACC_SUPER      = 0x0020,
  ACCESS_FLAGS__ACC_INTERFACE  = 0x0200,
  ACCESS_FLAGS__ACC_ABSTRACT   = 0x0400,
  ACCESS_FLAGS__ACC_SYNTHETIC  = 0x1000,
  ACCESS_FLAGS__ACC_ANNOTATION = 0x2000,
  ACCESS_FLAGS__ACC_ENUM       = 0x4000,
};

struct ClassFile {
  u4 magic;
  u2 minor_version;
  u2 major_version;
  u2 constant_pool_count;
  struct cp_info *constant_pool;
  u2 access_flags;
  u2 this_class;
  u2 super_class;
  u2 interfaces_count;
  u2 *interfaces;
  u2 fields_count;
  struct field_info *fields;
  u2 methods_count;
  struct method_info *methods;
  u2 attributes_count;
  struct attribute_info *attributes;
};

void uRead(FILE *f, void* addr, size_t n) {
  fread(f, sizeof(uint8_t), n, addr);
}

#define MAX_FRAME_STACK_SIZE 4096

struct frame {
  union jvm_variable* local_variables;
  // TODO: continue at jvms8.pdf, section 2.6.2
};

struct jvm_stack {
  uint32_t i;
  uint32_t max_size;
  // TODO: change this do dynamic alloc
  struct frame frames[MAX_FRAME_STACK_SIZE];
};

// @returns 1 if "stack overflow" error
// if dynamic alloc is used, should throw "out of memory" error if no mem
int jvm_stack_frame_push(struct jvm_stack* stack, const struct frame *frame) {
  stack->frames[stack->i++] = *frame;

  return stack->max_size > stack->i;
}

void jvm_stack_frame_pop(struct jvm_stack* stack) {
  stack->i--;
}

struct jvm_thread_info {
  uint32_t pc;
  struct jvm_stack *frame_stack;
};

// heap is shared between threads
// TODO: store reference count
// garbage collector appends free spaces (reference count == 0)
// to a DSU (disjoint set union). Heap alloc checks for free space in 
struct jvm_heap {
  void **mem;
  uint32_t size;
};

// returns pointer to allocated space
// if NULL, out of memory error
void* jvm_heap_alloc(uint32_t bytes, struct jvm_heap* heap) {
  heap->size++;
  void **new_mem = realloc(heap->mem, heap->size);
  if (new_mem == NULL) return NULL;

  new_mem[heap->size - 1] = malloc(bytes);
  if (new_mem[heap->size - 1] == NULL) return NULL;

  heap->mem = new_mem;

  return new_mem[heap->size - 1];
}

struct jvm_method_area {
  struct ClassFile** class_files;
  uint32_t size;
};

struct ClassFile* add_class_to_method_area(struct jvm_method_area* area, struct ClassFile* cf) {
  area->size++;

  struct ClassFile** new_mem = realloc(area->class_files, area->size);
  if (new_mem == NULL) return NULL;

  new_mem[area->size - 1] = cf;

  area->class_files = new_mem;

  return new_mem[area->size - 1];
}

void start_jvm_thread(struct ClassFile *cf) {
  
}

int main(
  int argc,
  char **argv
) {
  if (argc != 2) {
    printf(
      "You need to provide a single argument. The file that contains the program bytecodes.\n\
      Example:\n./bin minecraft.jar\n\n\t\
      If you wish to use a file in another directory, use the full path"
    );
    return 1;
  }

  FILE *filestream = fopen(argv[1], "rb");

  struct ClassFile *class_file = malloc(sizeof(struct ClassFile));

  uRead(filestream, &class_file->magic, 4);

  if (class_file->magic != 0xCAFEBABE) {
    printf("ERROR: provided file is not a Java .class file");
    return -1;
  }
  
  uRead(filestream, &class_file->minor_version, 2);
  uRead(filestream, &class_file->major_version, 2);
  if (class_file->major_version < 0) {
    // TODO
    printf("ERROR: Version not supported. bla bla");
    return -2;
  }
  uRead(filestream, &class_file->constant_pool_count, 2);
  if (
    // e.g., supports only til 100.0, 100.1 not supported anymore:
    class_file->major_version == 100  &&
    class_file->minor_version < 0
  ) {
    // TODO
    printf("ERROR: Version not supported. bla bla");
    return -2;
  }
  class_file->constant_pool = malloc(
    sizeof(struct cp_info) * (class_file->constant_pool_count - 1)
  );
  for (
    struct cp_info* cnst=class_file->constant_pool;
    cnst <= &class_file->constant_pool[class_file->constant_pool_count-1];
    cnst++
  ) {
    uRead(filestream, &cnst->tag, 1);
    // TAG__CONSTANT_Class = 7,
    // TAG__CONSTANT_Fieldref = 9,
    // TAG__CONSTANT_MethodRef = 10,
    // TAG__CONSTANT_InterfaceMethodRef = 11,
    // TAG__CONSTANT_String = 8,
    // TAG__CONSTANT_Integer = 3,
    // TAG__CONSTANT_Float = 4,
    // TAG__CONSTANT_Long = 5,
    // TAG__CONSTANT_Double = 6,
    // TAG__CONSTANT_NameAndType = 12,
    // TAG__CONSTANT_Utf8 = 1,
    // TAG__CONSTANT_MethodHandle = 15,
    // TAG__CONSTANT_MethodType = 16,
    // TAG__CONSTANT_InvokeDynamic = 18,
    switch (cnst->tag) {
      case TAG__CONSTANT_Class:
        uRead(filestream, &cnst->Class, sizeof(cnst->Class));
        break;
      case TAG__CONSTANT_Fieldref:
        uRead(filestream, &cnst->Fieldref, sizeof(cnst->Fieldref));
        break;
      case TAG__CONSTANT_MethodRef:
        uRead(filestream, &cnst->Methodref, sizeof(cnst->Methodref));
        break;
      case TAG__CONSTANT_InterfaceMethodRef:
        uRead(filestream, &cnst->InterfaceMethodref, sizeof(cnst->InterfaceMethodref));
        break;
      case TAG__CONSTANT_String:
        uRead(filestream, &cnst->String, sizeof(cnst->String));
        break;
      case TAG__CONSTANT_Integer:
        uRead(filestream, &cnst->Integer, sizeof(cnst->Integer));
        break;
      case TAG__CONSTANT_Float:
        uRead(filestream, &cnst->Float, sizeof(cnst->Float));
        break;
      case TAG__CONSTANT_Long:
        uRead(filestream, &cnst->Long, sizeof(cnst->Long));
        break;
      case TAG__CONSTANT_Double:
        uRead(filestream, &cnst->Double, sizeof(cnst->Double));
        break;
      case TAG__CONSTANT_NameAndType:
        uRead(filestream, &cnst->NameAndType, sizeof(cnst->NameAndType));
        break;
      case TAG__CONSTANT_Utf8:
        uRead(filestream, &cnst->Utf8, sizeof(cnst->Utf8));
        break;
      // TODO: add this constants to cp_info
      // case TAG__CONSTANT_MethodHandle:
      //   uRead(filestream, &cnst->MethodHandle, sizeof(cnst->MethodHandle));
      //   break;
      // case TAG__CONSTANT_MethodType:
      //   uRead(filestream, &cnst->MethodType, sizeof(cnst->MethodType));
      //   break;
      // case TAG__CONSTANT_InvokeDynamic:
      //   uRead(filestream, &cnst->InvokeDynamic, sizeof(cnst->InvokeDynamic));
      //   break;
    }
  }

  uRead(filestream, &class_file->access_flags, 2);
  uRead(filestream, &class_file->this_class, 2);
  uRead(filestream, &class_file->super_class, 2);

  uRead(filestream, &class_file->interfaces_count, 2);
  class_file->interfaces = malloc(sizeof(u2) * class_file->interfaces_count);
  uRead(filestream, &class_file->interfaces, 2 * class_file->interfaces_count);

  uRead(filestream, &class_file->fields_count, 2);
  class_file->fields = malloc(sizeof(struct field_info) * class_file->fields_count);
  uRead(filestream, &class_file->fields, sizeof(struct field_info) * class_file->fields_count);
  uRead(filestream, &class_file->methods_count, 2);
  class_file->methods = malloc(sizeof(struct method_info) * class_file->methods_count);
  uRead(filestream, &class_file->methods, sizeof(struct method_info) * class_file->methods_count);
  uRead(filestream, &class_file->attributes_count, 2);
  class_file->attributes = malloc(sizeof(struct attribute_info) * class_file->attributes_count);
  uRead(filestream, &class_file->attributes, sizeof(struct attribute_info) * class_file->attributes_count);

  fclose(filestream);

  struct jvm_global_info {
    struct jvm_method_area method_area;
  } global_info = {0};

  if (add_class_to_method_area(&global_info.method_area, class_file) == NULL) {
    perror("Out Of Memory");
    return 1;
  }

  start_jvm_thread(class_file);


  // TODO: run program

  // TODO: free memory

  // struct jvm_instance instance = jvm_create_instance();

  // int res = jvm_load_program_file(
  //   &instance,
  //   argv[1]
  // );

  // if (res) {
  //   printf("Ruim1\n");
  //   return res;
  // }

  // res = jvm_run_program(&instance);

  // if (res) {
  //   printf("Ruim1\n");
  //   return res;
  // }

  return 0;
}
