#include <elf.h>
#include <fs.h>
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
#define compare
#ifdef compare
extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern int fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);
static uintptr_t loader(PCB *pcb, const char *filename) {
  // Dev
  int fd = fs_open(filename, 0, 0);

  Log("load file %s, fd = %d", filename, fd);
  /* TODO(); */
  Elf_Ehdr ehdr;
  /* ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr)); */
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
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
  int phnum = ehdr.e_phnum;
  Elf_Phdr ph[ehdr.e_phnum];
  /* ramdisk_read(ph, ehdr.e_phoff, sizeof(Elf_Phdr) * phnum); */
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  assert(fs_read(fd, ph, sizeof(Elf_Phdr) * phnum) == sizeof(Elf_Phdr) * phnum);
  printf("phnum = %d\n", phnum);
  // 3.load program
  for (int i = 0; i < phnum; i++) {
    if (ph[i].p_type == PT_LOAD) {
      uintptr_t va = ph[i].p_vaddr;
      /* printf("PT_LOAD"); */
      /* printf("va = %x\n", va); */
      size_t off = ph[i].p_offset;
      size_t filesz = ph[i].p_filesz;
      size_t memsz = ph[i].p_memsz;
      /* ramdisk_read((void *)va, off, filesz); */
      fs_lseek(fd, off, SEEK_SET);
      assert(fs_read(fd, (void *)va, filesz) == filesz);
      if (filesz < memsz) {
        memset((void *)(va + filesz), 0, memsz - filesz);
      }
    }
  }
  fs_close(fd);
  return ehdr.e_entry;
}
#else
static uintptr_t loader(PCB *pcb, const char *filename) {
  // Elf_Ehdr ehdr; -
  // 声明一个Elf_Ehdr类型的结构体变量ehdr，用于存储ELF文件的头部信息。
  Elf_Ehdr ehdr;
  // 使用ramdisk_read函数从ramdisk中读取ELF文件的头部信息，从偏移量0开始读取sizeof(Elf_Ehdr)字节的数据。
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // 用于检查ELF文件的合法性
  // assert((*(uint32_t *)ehdr.e_ident == 0x7F454C46));
  // 创建一个Elf_Phdr类型的数组phdr，用于存储ELF文件的程序头表信息。ehdr.e_phnum表示头部表的数量
  Elf_Phdr phdr[ehdr.e_phnum];
  // 使用ramdisk_read函数从ramdisk中读取程序头表信息。ehdr.e_ehsize指示了程序头表在文件中的偏移量，
  // sizeof(Elf_Phdr)*ehdr.e_phnum表示要读取的字节数，将所有程序头表都读取到数组phdr中。
  ramdisk_read(phdr, ehdr.e_ehsize, sizeof(Elf_Phdr) * ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    // 检查当前程序头表条目的类型是否为PT_LOAD，表示这是一个需要加载到内存中的段
    if (phdr[i].p_type == PT_LOAD) {
      // 使用ramdisk_read函数将当前段的内容从ramdisk中读取到内存中。phdr[i].p_vaddr表示段的虚拟地址，
      // phdr[i].p_offset表示段在文件中的偏移量，phdr[i].p_memsz表示段在内存中的大小。
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      // 如果段的文件大小小于内存大小，这个代码用于将未初始化部分（即.bss部分）填充为零。
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0,
             phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  // 返回ELF文件的入口地址，表示加载并准备执行的程序的入口点。
  return ehdr.e_entry;
}

#endif
void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
