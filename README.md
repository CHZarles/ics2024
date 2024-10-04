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
