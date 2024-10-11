
#include "common.h"
#include <elf.h>
#include <utils.h>

#define MAX_FUNC 100
int func_cnt = 0;
Funcinfo funcinfo[MAX_FUNC];

char format_space[100];
int func_stack_top = 0;

void get_func_info(vaddr_t, vaddr_t *, char **);

void ftrace_call_func(vaddr_t source_addr, vaddr_t target_addr) {
  // check the pc_addr is in the range of function
  vaddr_t func_addr = 0;
  char *func_name = NULL;
  bool switch_func = false;
  for (int i = 0; i < func_cnt; i++) {
    if (funcinfo[i].value <= target_addr &&
        target_addr < funcinfo[i].value + funcinfo[i].size) {
      // if source_addr is in the function, then it is not a function call
      if (funcinfo[i].value <= source_addr &&
          source_addr < funcinfo[i].value + funcinfo[i].size) {
        continue;
      }
      func_addr = target_addr;
      func_name = funcinfo[i].func_name;
      switch_func = true;
      break;
    }
  }
  if (!switch_func) {
    return;
  }
  Assert(func_name != NULL, "Function name is NULL");
  Assert(func_stack_top < MAX_FUNC, "Function stack overflow");
  // display function call
  // 0x8000000c: call [_trm_init@0x80000260]
  // pc : call[funcname@funcaddr]
  func_stack_top++;
  format_space[func_stack_top * 2] = '\0';
  printf("%x :%s call[%s@%x]\n", target_addr, format_space, func_name,
         func_addr);
}
void ftrace_ret_func(vaddr_t source_addr, vaddr_t target_addr) {
  // check the pc_addr is in the range of function
  // check the pc_addr is in the range of function
  vaddr_t func_addr = 0;
  char *func_name = NULL;
  bool switch_func = false;
  for (int i = 0; i < func_cnt; i++) {
    if (funcinfo[i].value <= source_addr &&
        source_addr < funcinfo[i].value + funcinfo[i].size) {
      // if source_addr is in the function, then it is not a function call
      switch_func = true;
      if (funcinfo[i].value <= target_addr &&
          target_addr < funcinfo[i].value + funcinfo[i].size) {
        continue;
      }
      func_addr = source_addr;
      func_name = funcinfo[i].func_name;
      break;
    }
  }
  if (!switch_func) {
    return;
  }
  Assert(func_name != NULL, "Function name is NULL");
  Assert(func_stack_top > 0, "Function stack underflow");
  // display function return
  // 0x8000000c: ret [_trm_init@0x80000260]
  printf("func_stack_top: %d\n", func_stack_top);
  format_space[func_stack_top * 2] = '\0';
  func_stack_top--;
  printf("%x :%s ret[%s@%x]\n", target_addr, format_space, func_name,
         func_addr);
}

void get_func_info(vaddr_t addr, vaddr_t *func_addr, char **func_name) {
  for (int i = 0; i < func_cnt; i++) {
    if (funcinfo[i].value <= addr &&
        addr < funcinfo[i].value + funcinfo[i].size) {
      *func_addr = funcinfo[i].value;
      *func_name = funcinfo[i].func_name;
      return;
    }
  }
  Assert(0, "Failed to find function name");
}

int init_func_info(char *elf_file) {
  if (elf_file == NULL) {
    printf("Elf file is NULL\n");
    return EXIT_FAILURE;
  }
  // parse elf info from image file
  FILE *file = fopen(elf_file, "rb");
  if (file == NULL) {
    printf("Failed to open file %s", elf_file);
    perror("");
    return EXIT_FAILURE;
  } else {
    printf("Open file %s success\n", elf_file);
  }

  Elf32_Ehdr ehdr;
  if (fread(&ehdr, sizeof(ehdr), 1, file) != 1) {
    fprintf(stderr, "Failed to read ELF header\n");
    return EXIT_FAILURE;
  }

  fseek(file, ehdr.e_shoff, SEEK_SET);

  Elf32_Shdr shdr[ehdr.e_shnum];
  for (int i = 0; i < ehdr.e_shnum; i++) {
    if (fread(&shdr[i], sizeof(Elf32_Shdr), 1, file) != 1) {
      fprintf(stderr, "Failed to read section header\n");
      return EXIT_FAILURE;
    }
  }

  char *shstrtab = malloc(shdr[ehdr.e_shstrndx].sh_size);
  fseek(file, shdr[ehdr.e_shstrndx].sh_offset, SEEK_SET);
  if (fread(shstrtab, shdr[ehdr.e_shstrndx].sh_size, 1, file) != 1) {
    fprintf(stderr, "Failed to read section string table\n");
    return EXIT_FAILURE;
  }

  Elf32_Shdr *symtab_shdr = NULL, *strtab_shdr = NULL;
  printf("ehdr.e_shnum: %d\n", ehdr.e_shnum);
  for (int i = 0; i < ehdr.e_shnum; i++) {
    if (strcmp(shstrtab + shdr[i].sh_name, ".symtab") == 0) {
      symtab_shdr = &shdr[i];
    } else if (strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0) {
      strtab_shdr = &shdr[i];
    }
  }

  if (symtab_shdr == NULL || strtab_shdr == NULL) {
    fprintf(stderr, "Failed to find .symtab or .strtab\n");
    return EXIT_FAILURE;
  }

  char *symtab = malloc(symtab_shdr->sh_size);
  fseek(file, symtab_shdr->sh_offset, SEEK_SET);
  if (fread(symtab, symtab_shdr->sh_size, 1, file) != 1) {
    fprintf(stderr, "Failed to read symbol table\n");
    return EXIT_FAILURE;
  }

  char *strtab = malloc(strtab_shdr->sh_size);
  fseek(file, strtab_shdr->sh_offset, SEEK_SET);
  if (fread(strtab, strtab_shdr->sh_size, 1, file) != 1) {
    fprintf(stderr, "Failed to read string table\n");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < symtab_shdr->sh_size / sizeof(Elf32_Sym); i++) {
    Elf32_Sym *sym = (Elf32_Sym *)(symtab + i * sizeof(Elf32_Sym));
    if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC) {
      printf("Value: %x, Size: %x, Name: %s\n", sym->st_value, sym->st_size,
             strtab + sym->st_name);
      // save to funcinfo
      funcinfo[func_cnt].value = sym->st_value;
      funcinfo[func_cnt].size = sym->st_size;
      strcpy(funcinfo[func_cnt].func_name, strtab + sym->st_name);
      func_cnt++;
    }
  }

  free(shstrtab);
  free(symtab);
  free(strtab);
  fclose(file);

  // init format_space
  memset(format_space, ' ', sizeof(format_space));
  return EXIT_SUCCESS;
}
