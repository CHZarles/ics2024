
#include <elf.h>
#include <utils.h>

#define MAX_FUNC 100
int func_cnt = 0;
Funcinfo funcinfo[MAX_FUNC];

int func_stack_top = 0;

void get_func_info(vaddr_t, vaddr_t *, char **);

void ftrace_call_func(vaddr_t pc_addr) {

  Assert(func_stack_top < MAX_FUNC, "Function stack overflow");
  // display function call
  // 0x8000000c: call [_trm_init@0x80000260]
  // pc : call[funcname@funcaddr]
  vaddr_t func_addr;
  char *func_name;
  get_func_info(pc_addr, &func_addr, &func_name);
  char format_space[100];
  for (int i = 0; i < func_stack_top; i++) {
    format_space[i] = ' ';
  }
  format_space[func_stack_top] = '\0';
  printf("%x :%s call[%s@%x]\n", pc_addr, format_space, func_name, func_addr);
}
void ftrace_ret_func(vaddr_t pc_addr) {
  Assert(func_stack_top > 0, "Function stack underflow");
  // display function return
  // 0x8000000c: ret [_trm_init@0x80000260]
  vaddr_t func_addr;
  char *func_name;
  get_func_info(pc_addr, &func_addr, &func_name);
  char format_space[100];
  for (int i = 0; i < func_stack_top; i++) {
    format_space[i] = ' ';
  }
  format_space[func_stack_top] = '\0';
  printf("%x :%s ret[%s@%x]\n", pc_addr, format_space, func_name, func_addr);

  func_stack_top--;
}

void get_func_info(vaddr_t addr, vaddr_t *func_addr, char **func_name) {
  for (int i = 0; i < func_cnt; i++) {
    if (funcinfo[i].value <= addr &&
        addr < funcinfo[i].value + funcinfo[i].size) {
      *func_addr = funcinfo[i].value;
      *func_name = funcinfo[i].func_name;
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
  return EXIT_SUCCESS;
}
