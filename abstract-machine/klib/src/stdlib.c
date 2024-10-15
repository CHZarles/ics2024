#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) { next = seed; }

int abs(int x) { return (x < 0 ? -x : x); }

int atoi(const char *nptr) {
  int x = 0;
  while (*nptr == ' ') {
    nptr++;
  }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

// recored malloc location
extern Area heap;
uint8_t *malloc_position = NULL;
void *malloc(size_t size) {
  printf("call klib malloc\n");
  if (malloc_position == NULL)
    malloc_position = (void *)ROUNDUP(heap.start, 8);

  // On native, malloc() will be called during initialization of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
  /* #if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__)) */
  /* panic("Not implemented"); */
  // Check for overflow
  if ((uint8_t *)malloc_position + size > (uint8_t *)heap.end) {
    printf("malloc failed,  malloc_position %x, size %d\n", malloc_position,
           size);
    return NULL;
  }

  uint8_t *old_malloc_position = malloc_position;
  size = (size_t)ROUNDUP(size, 8);
  malloc_position += size;

  printf("malloc addr %x\n", malloc_position);

  return (void *)old_malloc_position;
  /* #endif */
  /*   printf( */
  /*       "Not meet !(defined(__ISA_NATIVE__) &&
   * defined(__NATIVE_USE_KLIB__))\n"); */
  /**/
  /*   return NULL; */
}

void free(void *ptr) {}

#endif
