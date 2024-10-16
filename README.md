# ICS2024 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2024/

To initialize, run

```bash
git clone -b 2024  git@github.com:CHZarles/ics2024.git
git branch -m master
bash init.sh nemu
bash init.sh abstract-machine
```

See `init.sh` for more details.

The following subprojects/components are included. Some of them are not fully implemented.

- [NEMU](https://github.com/NJU-ProjectN/nemu)
- [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
- [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
- [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)

# pa1 record

```
# generate compile_command.json
pip install compiledb
cd ./nemu
compiledb make

ref:
    https://pypi.org/project/compiledb/

# install node.js for copilot chat
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.0/install.sh | bash
export http_proxy=http://your_ip:7890
export https_proxy=https://your_ip:7890
export NVM_NODEJS_ORG_MIRROR=http://nodejs.org/dist
nvm list-remote
nvm install ...

ref:
    https://nodejs.org/en/download/package-manager

```

## readline example

```c++
// gcc readline_example.c -o readline -lreadline

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *input;

  while (1) {
    // 提示用户输入
    input = readline("Please enter a line of text (type 'exit' to quit): ");

    // 检查输入是否成功
    if (input) {
      // 如果输入是 "exit"，则退出循环
      if (strcmp(input, "exit") == 0) {
        free(input);
        break;
      }

      // 打印用户输入
      printf("You entered: %s\n", input);

      // 将输入添加到历史记录
      add_history(input);

      // 释放分配的内存
      free(input);
    } else {
      printf("Error reading input.\n");
      break; // 读取错误，退出循环
    }
  }

  return 0;
}


```

## strtok sample

```c++
// gcc -o strtok_example strtok_example.c
#include <stdio.h>
#include <string.h>

int main() {
  char str[] = "apple,banana,cherry,date";
  const char *delim = ",";
  char *token;

  // 获取第一个标记
  token = strtok(str, delim);

  // 遍历所有标记
  while (token != NULL) {
    printf("Token: %s\n", token);
    // 获取下一个标记
    token = strtok(NULL, delim);
  }

  return 0;
}

```

## strlen strcpy sample

```c++
//gcc -o strcpy_strlen_example strcpy_strlen_example.c

#include <stdio.h>
#include <string.h>

int main() {
  const char *source = "Hello, World!";
  char destination[50]; // 确保目标数组足够大

  // 使用 strcpy 复制字符串
  strcpy(destination, source);

  // 使用 strlen 计算字符串长度
  size_t length = strlen(destination);

  // 输出结果
  printf("Source: %s\n", source);
  printf("Destination: %s\n", destination);
  printf("Length of Destination: %zu\n", length);

  return 0;
}

```

## sscanf example

```c++
// gcc -o sscanf_example sscanf_example.c
#include <stdio.h>

int main() {
  const char *input = "John Doe 25";
  char name[50];
  int age;

  // 使用 sscanf 解析字符串
  int items_parsed = sscanf(input, "%49s %49s %d", name, name + 50, &age);

  // 检查解析结果
  if (items_parsed == 3) {
    printf("Name: %s %s\n", name, name + 50);
    printf("Age: %d\n", age);
  } else {
    printf("Failed to parse input.\n");
  }

  return 0;
}

```

## command si [N]

这里每次单步执行之后, 终端打印出来的是指令和 init.c 里面的数组的值是一致的

```

[src/monitor/monitor.c:34 welcome] Build time: 22:47:31, Sep 28 2024
Welcome to riscv32-NEMU!
For help, type "help"
(nemu) si 1
[src/monitor/sdb/sdb.c:72 step_n] Call step_n(0x587fdb875853)
[src/monitor/sdb/sdb.c:59 string_to_int] Call string_to_int(args = 0x587fdb875853, steps = 0x7ffd94175918)
[src/monitor/sdb/sdb.c:75 step_n] run 1 steps
0x80000000: 00 00 02 97 auipc   t0, 0
(nemu) si
[src/monitor/sdb/sdb.c:72 step_n] Call step_n((nil))
[src/monitor/sdb/sdb.c:59 string_to_int] Call string_to_int(args = (nil), steps = 0x7ffd94175918)
[src/monitor/sdb/sdb.c:62 string_to_int] *steps <- 1
[src/monitor/sdb/sdb.c:75 step_n] run 1 steps
0x80000004: 00 02 88 23 sb      zero, 0x10(t0)
(nemu)
(nemu) si
[src/monitor/sdb/sdb.c:72 step_n] Call step_n((nil))
[src/monitor/sdb/sdb.c:59 string_to_int] Call string_to_int(args = (nil), steps = 0x7ffd94175918)
[src/monitor/sdb/sdb.c:62 string_to_int] *steps <- 1
[src/monitor/sdb/sdb.c:75 step_n] run 1 steps
0x80000008: 01 02 c5 03 lbu     a0, 0x10(t0)
(nemu) si
[src/monitor/sdb/sdb.c:72 step_n] Call step_n((nil))
[src/monitor/sdb/sdb.c:59 string_to_int] Call string_to_int(args = (nil), steps = 0x7ffd94175918)
[src/monitor/sdb/sdb.c:62 string_to_int] *steps <- 1
[src/monitor/sdb/sdb.c:75 step_n] run 1 steps
0x8000000c: 00 10 00 73 ebreak
[src/cpu/cpu-exec.c:120 cpu_exec] nemu: HIT GOOD TRAP at pc = 0x8000000c
[src/cpu/cpu-exec.c:88 statistic] host time spent = 771 us
[src/cpu/cpu-exec.c:89 statistic] total guest instructions = 4
[src/cpu/cpu-exec.c:90 statistic] simulation frequency = 5,188 inst/s
(nemu)


```

# Pa2 record

```
YEMU
R0 R1 R2 R3 M7
33, 0 ,0 ,0 ,0
33, 33 ,0 ,0 ,0
16, 33 ,0 ,0 ,0
49, 33 ,0 ,0 ,0
49, 33 ,0 ,0 ,49
98, 33 ,0 ,0 ,49

指令（对应的二进制）和数据都保存在 内存M中
计算过程中的临时结果，保存在寄存器R 中

YEMU根据PC不断访问内存M中的某些指定位置，然后将这些位置的数据解释成指令，
再根据指令的行为来进行进行 对 内存 或 寄存器 中的数据进行操作



```

尝试RTFM并寻找这些内容的位置:

- 每一条指令具体行为的描述
- 指令opcode的编码表格

关于hart的解释
https://www.reddit.com/r/RISCV/comments/p967em/what_does_mean_harts/
https://stackoverflow.com/questions/70708319/user-thread-kernel-thread-software-thread-and-hardware-thread
https://www.risc-v1.com/thread-1070-1-1.html

vacant memory 是什么意思?
https://stackoverflow.com/questions/67822551/why-vacant-locations-in-the-address-space-are-never-accessible-in-risc-v-isa#:~:text=Different%20address%20ranges%20of%20a,these%20are%20called%20vacant%20locations.

[poe]
你现在是risc-v架构专家，我在读文档的时候遇到了一句不了解的话，To ensure that
certain implicit reads are ordered only after writes to the same memory locations, software must
execute specific fence or cache-control instructions defined for this purpose (such as the FENCE.I
instruction defined in Chapter 6). 你能解释一下吗

在RISC-V架构中，内存操作的顺序性是一个重要的概念，尤其是在多核处理器中。你提到的那句话主要涉及到内存屏障和缓存控制指令的使用，以确保特定的内存操作顺序。

解释：
隐式读取：
在执行某些指令时，处理器可能会隐式地读取数据。例如，在执行写操作后，处理器可能需要读取相同内存位置的值，以确保结果是最新的。
有序性：
为了保证程序的正确性，有时候需要确保在执行某个操作（如读取）之前，所有之前的写操作（对同一内存位置的写）都已经完成。这就是所谓的“内存顺序”。
FENCE指令：
FENCE指令用于创建内存屏障，确保特定的内存操作顺序。它可以防止指令的重排序，保证在它之前的所有写操作在它之后的所有读操作之前完成。
FENCE.I指令特指在指令缓存中创建屏障，确保所有之前的指令都已经执行完毕，适用于处理指令流的顺序问题。
缓存控制指令：
这些指令用于管理缓存的行为，确保数据在主内存和缓存之间的一致性。
总结：
为了确保程序在多核环境中的正确执行，开发者需要使用像FENCE.I这样的指令来明确控制内存操作的顺序。这可以防止由于硬件优化（如指令重排序）导致的数据不一致问题。

## key mask shift

```
bina   ??????? ????? ????? ??? ????? 00101 11
key    0000000 00000 00000 000 00000 00101 11
mask   0000000 00000 00000 000 00000 11111 11

shift  0

mask 是 key 的掩码, shift 是opcode 距离最低位的比特数量(似乎在risc-v中，shift 永远是 0)
```

## 整理一条指令在NEMU中的执行过程

cpu 这个全局变量(结构体) 保存pc 指针和 寄存器
pc 指针记录当次执行的指令的地址
在nemu启动的时候，pc 指针会被赋值到 RESET_VECTOR (0x80000000

typedef struct Decode {
vaddr_t pc;
vaddr_t snpc; // static next pc
vaddr_t dnpc; // dynamic next pc
ISADecodeInfo isa;
IFDEF(CONFIG_ITRACE, char logbuf[128]);
} Decode;

cpu.pc -> Decode -> 根据指令做对应的事情，

程序根据pc保存的地址来取指令，然后保存到 Decode 这个结构体里面
然后代码会解析， 指令的二进制 ，根据指令的语义，来修改pc或者寄存器的信息

## RTFSC, 理解执行未实现指令的时候, NEMU具体会怎么做.

```cpp
INSTPAT_START();
INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));

INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
* INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
INSTPAT_END();

```

如果一个指令没有实现，它会最后执行我上面代码 \* 好标记的那个宏
那个宏会调用下面这个函数

```cpp
void invalid_inst(vaddr_t thispc) {
  uint32_t temp[2];
  vaddr_t pc = thispc;
  temp[0] = inst_fetch(&pc, 4);
  temp[1] = inst_fetch(&pc, 4);

  uint8_t *p = (uint8_t *)temp;
  printf("invalid opcode(PC = " FMT_WORD "):\n"
      "\t%02x %02x %02x %02x %02x %02x %02x %02x ...\n"
      "\t%08x %08x...\n",
      thispc, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], temp[0], temp[1]);

  printf("There are two cases which will trigger this unexpected exception:\n"
      "1. The instruction at PC = " FMT_WORD " is not implemented.\n"
      "2. Something is implemented incorrectly.\n", thispc);
  printf("Find this PC(" FMT_WORD ") in the disassembling result to distinguish which case it is.\n\n", thispc);
  printf(ANSI_FMT("If it is the first case, see\n%s\nfor more details.\n\n"
        "If it is the second case, remember:\n"
        "* The machine is always right!\n"
        "* Every line of untested code is always wrong!\n\n", ANSI_FG_RED), isa_logo);

  set_nemu_state(NEMU_ABORT, thispc, -1);
}
```

这个函数的功能是打印log信息，并且设置nemu的状态为abort

## JAL 命令解惑

https://www.reddit.com/r/RISCV/comments/1e5aa9w/quick_question_about_riscv_instruction/

## 辅助工具

https://luplab.gitlab.io/rvcodecjs/#q=000aa903&abi=false&isa=AUTO

https://nju-projectn.github.io/dlco-lecture-note/exp/11.html

## 两个宏的测试

```c++
#include <bitset>
#include <cstdint>
#include <iostream>

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))

int main() {
  uint64_t x = 0b110110; // 二进制数 110110

  std::cout << "1ull << 3: " << std::bitset<8>(1ull << 3)
            << std::endl; // 期望输出 0b1000
  std::cout << "(1ull << 3) - 1: " << std::bitset<8>((1ull << 3) - 1)
            << std::endl; // 期望输出 0b111
  // 测试 BITMASK 宏
  std::cout << "BITMASK(3): " << std::bitset<8>(BITMASK(3))
            << std::endl; // 期望输出 0b111
  std::cout << "BITMASK(5): " << std::bitset<8>(BITMASK(5))
            << std::endl; // 期望输出 0b11111

  // 测试 BITS 宏
  std::cout << "BITS(x, 4, 2): " << std::bitset<8>(BITS(x, 4, 2))
            << std::endl; // 期望输出 0b101
  std::cout << "BITS(x, 5, 1): " << std::bitset<8>(BITS(x, 5, 1))
            << std::endl; // 期望输出 0b11011

  return 0;
}
```

## pa2.2部分很有意思

按我的理解，实验设计者把整个计算机系统分成了

硬件裸机（bare-metal） + 抽象计算机（abstract machine, 下面简称AM） 两部分

先前实验实现的 nemu 其实就是充当一个裸机的角色

裸机可以用 （ISA，平台）表示，无论如果，逻辑提供了一个 图灵机(trm) 的角色

裸机提供的最简单的 运行时环境 ：1.把程序放在正确（约定）的位置 （说明有内存）2.计算机自动执行。（指令实现正确3.程序会结束 （指令实现正确

这个运行时环境是和平台相关的

AM作为各种“高级软件”和硬件的中间层，首先要实现的就是 TRM api

最简单的 TRM api 的功能应该有有 - 初始化trm - 通知 trm 程序运行结束 - 提供内存访问的起始地址和结束地址

只要实现了上面的功能，那么之后我们就可以直接基于AM的trm api来进行开发更复杂的功能
而不用在乎“硬件相关的实现”

## 实现ftrace

我发现x86下编译出来的文件的elf和nemu编译出来的非常不一样。后者的要简单不少。
这个部分的任务只要关注，

0. 读 section header， 找到 .symtab 和 .strtab
1. 读入 .symtab , 找出类型为 FUNC 的项，
   记录下对应的 Value（函数起始地址）， Size（函数大小）, Name(这个函数的名字在字符串表中的起始位置)

2. 根据步骤一得到的这些东西，在strtab中查函数名

````

首先，我们需要包含必要的头文件，然后打开 ELF 文件并读取其 section header。我们需要找到 .symtab 和 .strtab 的 section header，然后读取这些 section 的内容。对于 .symtab，我们需要找到类型为 FUNC 的项，并记录下它们的 Value（函数起始地址），Size（函数大小）和 Name（函数名在字符串表中的起始位置）。然后，我们可以使用这些信息在 .strtab 中查找函数名。

以下是实现这个功能的 C 代码：

```c
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    Elf64_Ehdr ehdr;
    fread(&ehdr, sizeof(ehdr), 1, file);

    fseek(file, ehdr.e_shoff, SEEK_SET);

    Elf64_Shdr shdr[ehdr.e_shnum];
    for (int i = 0; i < ehdr.e_shnum; i++) {
        fread(&shdr[i], sizeof(Elf64_Shdr), 1, file);
    }

    char *strtab = malloc(shdr[ehdr.e_shstrndx].sh_size);
    fseek(file, shdr[ehdr.e_shstrndx].sh_offset, SEEK_SET);
    fread(strtab, shdr[ehdr.e_shstrndx].sh_size, 1, file);

    Elf64_Shdr *symtab_shdr = NULL, *strtab_shdr = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (strcmp(strtab + shdr[i].sh_name, ".symtab") == 0) {
            symtab_shdr = &shdr[i];
        } else if (strcmp(strtab + shdr[i].sh_name, ".strtab") == 0) {
            strtab_shdr = &shdr[i];
        }
    }

    if (symtab_shdr == NULL || strtab_shdr == NULL) {
        fprintf(stderr, "Failed to find .symtab or .strtab\n");
        return EXIT_FAILURE;
    }

    char *symtab = malloc(symtab_shdr->sh_size);
    fseek(file, symtab_shdr->sh_offset, SEEK_SET);
    fread(symtab, symtab_shdr->sh_size, 1, file);

    char *strtab2 = malloc(strtab_shdr->sh_size);
    fseek(file, strtab_shdr->sh_offset, SEEK_SET);
    fread(strtab2, strtab_shdr->sh_size, 1, file);

    for (int i = 0; i < symtab_shdr->sh_size / sizeof(Elf64_Sym); i++) {
        Elf64_Sym *sym = (Elf64_Sym *)(symtab + i * sizeof(Elf64_Sym));
        if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC) {
            printf("Value: %lx, Size: %lx, Name: %s\n", sym->st_value, sym->st_size, strtab2 + sym->st_name);
        }
    }

    free(strtab);
    free(symtab);
    free(strtab2);
    fclose(file);

    return EXIT_SUCCESS;
}
````

这个程序需要 ELF 文件的路径作为命令行参数。它首先打开文件并读取 ELF header。然后，它读取所有的 section header 并找到 .symtab 和 .strtab。然后，它读取这两个 section 的内容。对于 .symtab，它遍历所有的 symbol，并检查它们的类型。如果类型是 FUNC，它就打印出 symbol 的 Value，Size 和 Name。

## User ───

请你逐句解释这份代码

## Copilot ───

这段代码是用C语言编写的，用于读取ELF文件的.symtab和.strtab部分，并找出类型为FUNC的项。下面是对每一部分的解释：

```c
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

这部分代码包含了必要的头文件。`elf.h`是用于处理ELF格式文件的头文件，`stdio.h`、`stdlib.h`和`string.h`是标准C库的头文件。

```c
int main(int argc, char **argv) {
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }
```

这部分代码定义了程序的主函数。它打开一个文件，该文件的路径是从命令行参数中获取的。如果文件打开失败，它将打印错误信息并退出程序。

```c
    Elf64_Ehdr ehdr;
    fread(&ehdr, sizeof(ehdr), 1, file);
```

这部分代码定义了一个`Elf64_Ehdr`结构体变量`ehdr`，并从文件中读取ELF头到这个变量。

```c
    fseek(file, ehdr.e_shoff, SEEK_SET);
```

这部分代码将文件的位置指针设置到section header的开始位置。

```c
    Elf64_Shdr shdr[ehdr.e_shnum];
    for (int i = 0; i < ehdr.e_shnum; i++) {
        fread(&shdr[i], sizeof(Elf64_Shdr), 1, file);
    }
```

这部分代码定义了一个`Elf64_Shdr`结构体数组`shdr`，并从文件中读取所有的section header到这个数组。

```c
    char *strtab = malloc(shdr[ehdr.e_shstrndx].sh_size);
    fseek(file, shdr[ehdr.e_shstrndx].sh_offset, SEEK_SET);
    fread(strtab, shdr[ehdr.e_shstrndx].sh_size, 1, file);
```

这部分代码分配了一块内存用于存储section header string table，并从文件中读取这个table到分配的内存。

```c
    Elf64_Shdr *symtab_shdr = NULL, *strtab_shdr = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (strcmp(strtab + shdr[i].sh_name, ".symtab") == 0) {
            symtab_shdr = &shdr[i];
        } else if (strcmp(strtab + shdr[i].sh_name, ".strtab") == 0) {
            strtab_shdr = &shdr[i];
        }
    }
```

这部分代码遍历所有的section header，找到.symtab和.strtab的section header，并保存它们的指针。

```c
    if (symtab_shdr == NULL || strtab_shdr == NULL) {
        fprintf(stderr, "Failed to find .symtab or .strtab\n");
        return EXIT_FAILURE;
    }
```

这部分代码检查是否找到了.symtab和.strtab的section header，如果没有找到，它将打印错误信息并退出程序。

```c
    char *symtab = malloc(symtab_shdr->sh_size);
    fseek(file, symtab_shdr->sh_offset, SEEK_SET);
    fread(symtab, symtab_shdr->sh_size, 1, file);
```

这部分代码分配了一块内存用于存储.symtab，并从文件中读取这个section到分配的内存。

```c
    char *strtab2 = malloc(strtab_shdr->sh_size);
    fseek(file, strtab_shdr->sh_offset, SEEK_SET);
    fread(strtab2, strtab_shdr->sh_size, 1, file);
```

这部分代码分配了一块内存用于存储.strtab，并从文件中读取这个section到分配的内存。

```c
    for (int i = 0; i < symtab_shdr->sh_size / sizeof(Elf64_Sym); i++) {
        Elf64_Sym *sym = (Elf64_Sym *)(symtab + i * sizeof(Elf64_Sym));
        if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC) {
            printf("Value: %lx, Size: %lx, Name: %s\n", sym->st_value, sym->st_size, strtab2 + sym->st_name);
        }
    }
```

这部分代码遍历.symtab中的所有symbol，找到类型为FUNC的symbol，并打印它们的Value，Size和Name。

```c
    free(strtab);
    free(symtab);
    free(strtab2);
    fclose(file);

    return EXIT_SUCCESS;
}
```

这部分代码释放了分配的内存，并关闭了文件。然后，它返回成功状态码，表示程序正常结束。

```


```

# pa3 record

## 实现中断陷入

imm 和 csr 的映射，参考 priv-isa-asciidoc.pdf ，page 17
c 语言没用hash table的标准库，所以这里还不能直接用映射实现
