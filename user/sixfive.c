#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const char *SEPS = " -\r\t\n./,";

// Parse a string of digits of length len into an int.
// Return -1 if any non-digit is found.
static int
parseint(const char *s, int len)
{
  int v = 0;
  for (int i = 0; i < len; i++) {
    char c = s[i];
    if (c < '0' || c > '9')
      return -1;
    v = v * 10 + (c - '0');
  }
  return v;
}

// If we have a valid token, print it if divisible by 5 or 6.
static void
maybe_print(const char *tok, int len, int alldigits)
{
  if (len == 0 || !alldigits)
    return;

  int v = parseint(tok, len);
  if (v >= 0 && (v % 5 == 0 || v % 6 == 0)) {
    printf("%d\n", v);
  }
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: sixfive file...\n");
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], 0);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }

    char tok[64];
    int tlen = 0;
    int alldigits = 1;
    char c;
    int r;

    while ((r = read(fd, &c, 1)) > 0) {
      if (strchr(SEPS, c)) {
        // separator ends the current token
        maybe_print(tok, tlen, alldigits);
        tlen = 0;
        alldigits = 1;
      } else {
        if (tlen < (int)sizeof(tok)) {
          tok[tlen++] = c;
          if (c < '0' || c > '9')
            alldigits = 0;
        } else {
          alldigits = 0; // token too long
        }
      }
    }

    // EOF is also a separator → flush the last token
    maybe_print(tok, tlen, alldigits);

    close(fd);
  }

  exit(0);
}

