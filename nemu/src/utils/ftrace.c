
#include <utils.h>

#define MAX_FUNC 100
Funcinfo funcinfo[MAX_FUNC];

char *get_func_name(vaddr_t addr) {
  for (int i = 0; i < MAX_FUNC; i++) {
    if (funcinfo[i].value == addr) {
      return funcinfo[i].func_name;
    }
  }
  return NULL;
}

void save_func_info(char *img_file) {
  // parse elf info from image file
}
