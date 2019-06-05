#include "debug.h"
#include <cstdio>
#include <cstdarg>
#include <string.h>

// デバッグ出力
// printfと同じ引数を取る
void debug(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}
