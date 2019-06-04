#include <stdio.h>

extern int test1();
extern int test2();

int test1() {
  printf("This is C language.\nOK\n");
  return 5;
}

int test2(int x, int y) {
  printf("This is C language. x = %d, y = %d\nOK\n", x, y);
  return x + y;
}
