#include <elf.h>
#include <proc.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
static uintptr_t loader(PCB *pcb, const char *filename) {
  /* TODO(); */
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // 1.check magic head number and ISA
  printf("ehdr.e_ident = %x\n", *(uint32_t *)(ehdr.e_ident));
  assert(*(uint32_t *)(ehdr.e_ident) == 0x464c457f);

// 这些宏定义在xxx/libos/crt0/start.S中
#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
#define EXPECT_TYPE EM_386
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_MIPS32__)
#define EXPECT_TYPE EM_MIPS
#else
#error Unsupported ISA
#endif
  assert(ehdr.e_machine == EXPECT_TYPE);
  // 2.get program table header
  Elf_Phdr ph_;
  ramdisk_read(&ph_, ehdr.e_phoff, sizeof(Elf_Phdr));
  int phnum = ehdr.e_phnum;
  printf("phnum = %d\n", phnum);
  Elf_Phdr *ph = &ph_;
  // 3.load program
  for (int i = 0; i < phnum; i++) {
    if (ph[i].p_type == PT_LOAD) {
      uintptr_t va = ph[i].p_vaddr;
      /* printf("PT_LOAD"); */
      /* printf("va = %x\n", va); */
      size_t off = ph[i].p_offset;
      size_t filesz = ph[i].p_filesz;
      size_t memsz = ph[i].p_memsz;
      ramdisk_read((void *)va, off, filesz);
      if (filesz < memsz) {
        memset((void *)(va + filesz), 0, memsz - filesz);
      }
    }
  }

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
