#include <stdio.h>
#include <stdlib.h>

void alloc4(int **p, int v1, int v2, int v3, int v4) {
  *p = (int*)malloc(sizeof(int) * 4);
  (*p)[0] = v1;
  (*p)[1] = v2;
  (*p)[2] = v3;
  (*p)[3] = v4;
}

int test1() {
  printf("This is C language.\nOK\n");
  return 5;
}

int test2(int x, int y, int z) {
  printf("This is C language. x = %d, y = %d, z = %d\nOK\n", x, y, z);
  return x + y + z;
}

int test3(int p1, int p2, int p3, int p4, int p5, int p6, int p7) {
  printf("This is C language. p1 = %d, p2 = %d, p3 = %d, p4 = %d, p5 = %d, p6 = %d, p7 = %d\nOK\n", p1, p2, p3, p4, p5, p6, p7);
  return p1 + p2 + p3 + p4 + p5 + p6 + p7;
}
