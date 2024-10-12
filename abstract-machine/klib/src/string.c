#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  /* The  strlen()  function calculates the length of the string pointed to by
   * s,  */
  /* excluding the terminating null byte ('\0'). */
  size_t len = 0;
  while (*s != '\0') {
    len++;
    s++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {

  /* The  strcpy() function copies the string pointed to by src, including the
   * termi‐ */
  /* nating null byte ('\0'), to the buffer pointed to by dest.  The strings may
   * not */
  /* overlap,  and  the  destination  string dest must be large enough to
   * receive the */
  /* copy.  Beware of buffer overruns!  (See BUGS.) */
  char *ret = dst;
  while (*src != '\0') {
    *dst = *src;
    dst++;
    src++;
  }
  *dst = '\0';
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  /* The strncpy() function is similar, except that  at  most  n  bytes  of  src
   * are */
  /* copied.   Warning:  If there is no null byte among the first n bytes of
   * src, the */
  /* string placed in dest will not be null-terminated. */
  /**/
  /* If the length of src is less than n, strncpy() writes additional null bytes
   * to */
  /* dest to ensure that a total of n bytes are written. */
  char *ret = dst;
  while (n > 0 && *src != '\0') {
    *dst = *src;
    dst++;
    src++;
    n--;
  }
  if (n > 0) {
    *dst = '\0';
  }
  return ret;
}

char *strcat(char *dst, const char *src) {

  /* The strcat() function appends the src string to the dest string,
   * overwriting the */
  /* terminating null byte ('\0') at the end of dest, and  then  adds  a
   * terminating */
  /* null  byte.   The  strings may not overlap, and the dest string must have
   * enough */
  /* space for the result.  If dest is not large enough, program behavior  is
   * unpre‐ */
  /* dictable; buffer overruns are a favorite avenue for attacking secure
   * programs. */

  char *ret = dst;
  while (*dst != '\0') {
    dst++;
  }

  while (*src != '\0') {
    *dst = *src;
    dst++;
    src++;
  }
  *dst = '\0';
  return ret;
}

int strcmp(const char *s1, const char *s2) {

  /* The  strcmp()  function  compares  the two strings s1 and s2.  The locale
   * is not */
  /* taken into account (for a locale-aware comparison, see strcoll(3)).  The
   * compar‐ */
  /* ison is done using unsigned characters. */
  /**/
  /* strcmp() returns an integer indicating the result of the comparison, as
   * follows: */
  /**/
  /* • 0, if the s1 and s2 are equal; */
  /**/
  /* • a negative value if s1 is less than s2; */
  /**/
  /* • a positive value if s1 is greater than s2. */
  /**/
  /**/
  // special case
  int ret = 0;

  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 != *s2) {
      ret = *s1 - *s2;
      break;
    }
    s1++;
    s2++;
  }
  if (*s1 == '\0' && *s2 != '\0') {
    ret = -1;
  } else if (*s1 != '\0' && *s2 == '\0') {
    ret = 1;
  }
  return ret;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  /* The strncmp() function is similar, except it compares only the first (at
   * most) n */
  /* bytes of s1 and s2. */
  int ret = 0;
  int count = n;
  while (count-- > 0 && *s1 != '\0' && *s2 != '\0') {
    if (*s1 != *s2) {
      ret = *s1 - *s2;
      break;
    }
    s1++;
    s2++;
  }

  if (*s1 == '\0' && *s2 != '\0') {
    ret = -1;
  } else if (*s1 != '\0' && *s2 == '\0') {
    ret = 1;
  }
  return ret;
}

void *memset(void *s, int c, size_t n) {

  /* DESCRIPTION */
  /*        The memset() function fills the first n bytes of the memory area
   * pointed to by s */
  /*        with the constant byte c. */
  /**/
  /* RETURN VALUE */
  /*        The memset() function returns a pointer to the memory area s. */
  /**/
  void *ret = s;
  while (n-- > 0) {
    *(char *)s = c;
    s++;
  }
  return ret;
}

void *memmove(void *dst, const void *src, size_t n) {

  /* DESCRIPTION */
  /*        The  memmove() function copies n bytes from memory area src to
   * memory area dest. */
  /*        The memory areas may overlap: copying takes place as though the
   * bytes in src are */
  /*        first  copied  into a temporary array that does not overlap src or
   * dest, and the */
  /*        bytes are then copied from the temporary array to dest. */
  /**/
  /* RETURN VALUE */
  /*        The memmove() function returns a pointer to dest. */
  void *ret = dst;
  // pay attention , use trm api here
  void *tmp = malloc(n);

  int count = n;
  while (count-- > 0) {
    *(char *)tmp = *(char *)src;
    src++;
    tmp++;
  }
  count = n;
  while (count-- > 0) {
    *(char *)dst = *(char *)tmp;
    dst++;
    tmp++;
  }
  /* panic("Not implemented"); */
  return ret;
}

void *memcpy(void *out, const void *in, size_t n) {

  /* DESCRIPTION */
  /*        The  memcpy()  function copies n bytes from memory area src to
   * memory area dest. */
  /*        The memory areas must not overlap.  Use memmove(3) if the memory
   * areas do  over‐ */
  /*        lap. */
  /**/
  /* RETURN VALUE */
  /*        The memcpy() function returns a pointer to dest. */

  void *ret = out;
  int count = n;
  while (count-- > 0) {
    *(char *)out = *(char *)in;
    out++;
    in++;
  }
  return ret;
  /* panic("Not implemented");  */
}

int memcmp(const void *s1, const void *s2, size_t n) {

  /* DESCRIPTION */
  /*        The  memcmp()  function compares the first n bytes (each interpreted
   * as unsigned char) of the memory */
  /*        areas s1 and s2. */
  /**/
  /* RETURN VALUE */
  /*        The memcmp() function returns an integer less than, equal to, or
   * greater than zero if  the  first  n */
  /*        bytes  of s1 is found, respectively, to be less than, to match, or
   * be greater than the first n bytes */
  /*        of s2. */
  /**/
  /*        For a nonzero return value, the sign is determined by the sign of
   * the difference between  the  first */
  /*        pair of bytes (interpreted as unsigned char) that differ in s1 and
   * s2. */
  /**/
  /*        If n is zero, the return value is zero. */

  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] - p2[i];
    }
  }
  return 0;
}

#endif
