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
  ramdisk_read(&ehdr, 0, sizeof(Elf_Phdr));
  // 1.check magic head number and ISA
  assert(*(uint32_t *)(ehdr.e_ident) == 0x7f454c46);

#if defined(__ISA_AM_NATIVE__)
  // TODO
  assert(ehdr.e_machine == EM_X86_64);
#else // riscv
  // TODO
  assert(ehdr.e_machine == EM_RISCV);
#endif

  // 2.get program table header
  Elf_Phdr ph_;
  ramdisk_read(&ph_, ehdr.e_phoff, sizeof(Elf_Phdr));
  int phnum = ehdr.e_phnum;
  Elf_Phdr *ph = &ph_;
  // 3.load program
  for (int i = 0; i < phnum; i++) {
    if (ph[i].p_type == PT_LOAD) {
      uintptr_t va = ph[i].p_vaddr;
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
