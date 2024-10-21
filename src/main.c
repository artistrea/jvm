#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jvm.h"
#include "jvm_utils.h"

void fs_path_join(char* destination, const char* abs, const char* relative) {
#ifdef WIN32
  char separator = '\\';
#else
  char separator = '/';
#endif

}

int main(
  int argc,
  char **argv
) {
  struct jvm_instance instance = jvm_create_instance();

  if (argc != 2) {
    printf(
      "You need to provide a single argument. The file that contains the program bytecodes.\n\
      Example:\n./bin minecraft.jar\n\n\t\
      If you wish to use a file in another directory, use the full path"
    );
  }

  int res = jvm_load_program_file(
    &instance,
    argv[1]
  );

  if (res) return res;

  res = jvm_run_program(&instance);

  if (res) return res;

  return 0;
}
