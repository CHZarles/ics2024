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
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)

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
