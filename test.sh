#!/bin/bash

try() {
  expected="$1"
  input="$2"

  echo "$input"
  ./cppm "$input" > tmp.s
  gcc -g -c tmp.s
  gcc -g -c test.c
  gcc -g -o tmp tmp.o test.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual <= $input"
    exit 1
  fi
}

# ユニットテストの作成
try 0 "int main(int argc, int argv)  { return 0; }"
try 42 "int main(int argc, int argv)  { return 42; }"
# ステップ2：加減算のできるコンパイラの作成
try 21 "int main(int argc, int argv) { return 5+20-4; }"
# ステップ3：トークナイザを導入
try 41 "int main(int argc, int argv)  {  return 12 + 34 - 5 ; }"
# 生成規則による演算子の優先順位の表現
try 14 "int main(int argc, int argv)  { return 1 * 2 + 3 * 4; }"
# ステップ4：四則演算のできる言語の作成
try 47 "int main(int argc, int argv)  { return 5+6*7; }"
try 15 "int main(int argc, int argv)  { return 5*(9-6); }"
try 4 "int main(int argc, int argv)  { return (3+5)/2; }"
# ステップ5：単項プラスと単項マイナス
try 3 "int main(int argc, int argv)  { return -5 * 3 - +2 + -5 * -4; }"
# ステップ6: 比較演算子
try 1 "int main(int argc, int argv)  { return 2 + 3 * 1 == 5; }"
try 0 "int main(int argc, int argv)  { return 2 == 3; }"
try 1 "int main(int argc, int argv)  { return 2 != 3; }"
try 0 "int main(int argc, int argv)  { return 3 * 0 != 5 * 0 + 0; }"
try 1 "int main(int argc, int argv)  { return 1 > 0; }"
try 0 "int main(int argc, int argv)  { return 0 > 0; }"
try 1 "int main(int argc, int argv)  { return 0 < 1; }"
try 0 "int main(int argc, int argv)  { return 0 < 0; }"
try 1 "int main(int argc, int argv)  { return 1 >= 0; }"
try 1 "int main(int argc, int argv)  { return 0 >= 0; }"
try 1 "int main(int argc, int argv)  { return 0 <= 1; }"
try 1 "int main(int argc, int argv)  { return 0 <= 0; }"
# ステップ9：1文字のローカル変数
#try 15 "int main(int argc, int argv)  { int a; a = 3; int b; b = 30; int c; c = 5; b - a * c; }"
# ステップ10：return文
try 15 "int main(int argc, int argv)  { int a; a = 3; int b; b = 30; int c; c = 5; return b - a * c; }"
# ステップ11：複数文字のローカル変数
try 15 "int main(int argc, int argv)  { int foo; foo = 3; int bar; bar = 30; int baz; baz = 5; return bar - foo * baz; }"
try 3 "int main(int argc, int argv)  { int foo; foo = 1; int bar; bar = 2; int baz; baz = 3; return (foo == 1) + (bar == 2) + (baz == 3); }"
try 10 "int main(int argc, int argv)  { int foo; foo = 1; foo = foo + 1; foo = foo + 5; foo = 3 + foo; return foo; }"
try 0 "int main(int argc, int argv)  { int a; a = 0; return a; }"
# ステップ12: 制御構文を足す
# if
try 2 "int main(int argc, int argv)  { int a; a = 5; if (2 > 1) a = 2; return a; }"
try 5 "int main(int argc, int argv)  { int a; a = 5; if (2 < 1) a = 2; return a; }"
# if else
try 2 "int main(int argc, int argv)  { int a; a = 5; if (2 > 1) a = 2; else a = 3; return a; }"
try 3 "int main(int argc, int argv)  { int a; a = 5; if (2 < 1) a = 2; else a = 3; return a; }"
# while
try 0 "int main(int argc, int argv)  { int i; i = 5; while (i > 0) i = i - 1; return i; }"
# for
try 6 "int main(int argc, int argv)  { int a; int i; a = 1; for (i = 0; i < 5; i = i + 1) a = a + 1; return a; }"
# ステップ13: ブロック
try 21 "int main(int argc, int argv)  { int a; a = 5; if (2 > 1) { a = 20; a = a + 1; } else { a = 30; a = a - 1; } return a; }"
try 29 "int main(int argc, int argv)  { int a; a = 5; if (2 < 1) { a = 20; a = a + 1; } else { a = 30; a = a - 1; } return a; }"
# ステップ14: 関数の呼び出しに対応する
try 5 "int main(int argc, int argv)  { return test1(); }"
try 12 "int main(int argc, int argv)  { return test2(3, 4, 5); }"
try 28 "int main(int argc, int argv)  { return test3(1, 2, 3, 4, 5, 6, 7); }"
# ステップ15: 関数の定義に対応する
try 5 "int main(int argc, int argv) { return 5; }"
try 7 "int test0(int x, int y) { return x + y; } int main(int argc, int argv) { return test0(3, 4); }"
try 180 "int test0(int p1, int p2, int p3, int p4, int p5, int p6, int p7) { return (p1 + p2 + p3 + p4) * (p5 + p6 + p7); } int main(int argc, int argv) { return test0(1, 2, 3, 4, 5, 6, 7); }"
try 120 "int factorial(int n) { if (n <= 1) return 1; else return n * factorial(n - 1); } int main(int argc, int argv) { return factorial(5); }"
try 120 "int factorial(int n) { if (n <= 1) return 1; return n * factorial(n - 1); } int main(int argc, int argv) { return factorial(5); }"
# ステップ17: ポインタ型を導入する
try 0 "int main(int argc, int argv) { int* a; return 0; }"
try 0 "int main(int argc, int argv) { int* a; return 0; } int test(int *a) {}"
try 0 "int main(int argc, int argv) { int* a; return 0; } int *test(int a) {}"
try 3 "int main(int argc, int argv) { int x; x = 3; int *y; y = &x; return *y; }"
# ステップ18: ポインタの加算と減算を実装する
try 1 "int main(int argc, int argv) { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p; return *q; }"
try 4 "int main(int argc, int argv) { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; return *q; }"
try 8 "int main(int argc, int argv) { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 3; return *q; }"

echo OK
