#!/bin/bash

try() {
  expected="$1"
  input="$2"

  ./cppm "$input" > tmp.s
  # gcc -o tmp tmp.s
  gcc -c tmp.s
  gcc -c test.c
  gcc -o tmp tmp.o test.o
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
try 0 "return 0;"
try 42 "return 42;"
# ステップ2：加減算のできるコンパイラの作成
try 21 'return 5+20-4;'
# ステップ3：トークナイザを導入
try 41 " return 12 + 34 - 5 ;"
# 生成規則による演算子の優先順位の表現
try 14 "return 1 * 2 + 3 * 4;"
# ステップ4：四則演算のできる言語の作成
try 47 "return 5+6*7;"
try 15 "return 5*(9-6);"
try 4 "return (3+5)/2;"
# ステップ5：単項プラスと単項マイナス
try 3 "return -5 * 3 - +2 + -5 * -4;"
# ステップ6: 比較演算子
try 1 "return 2 + 3 * 1 == 5;"
try 0 "return 2 == 3;"
try 1 "return 2 != 3;"
try 0 "return 3 * 0 != 5 * 0 + 0;"
try 1 "return 1 > 0;"
try 0 "return 0 > 0;"
try 1 "return 0 < 1;"
try 0 "return 0 < 0;"
try 1 "return 1 >= 0;"
try 1 "return 0 >= 0;"
try 1 "return 0 <= 1;"
try 1 "return 0 <= 0;"
# ステップ9：1文字のローカル変数
try 15 "a = 3; b = 30; c = 5; b - a * c;"
# ステップ10：return文
try 15 "a = 3; b = 30; c = 5; return b - a * c;"
# ステップ11：複数文字のローカル変数
try 15 "foo = 3; bar = 30; baz = 5; return bar - foo * baz;"
try 3 "foo = 1; bar = 2; baz = 3; return (foo == 1) + (bar == 2) + (baz == 3);"
try 10 "foo = 1; foo = foo + 1; foo = foo + 5; foo = 3 + foo; return foo;"
try 0 "a = 0; return a;"
# ステップ12: 制御構文を足す
# if
try 2 "a = 5; if (2 > 1) a = 2; return a;"
try 5 "a = 5; if (2 < 1) a = 2; return a;"
# if else
try 2 "a = 5; if (2 > 1) a = 2; else a = 3; return a;"
try 3 "a = 5; if (2 < 1) a = 2; else a = 3; return a;"
# while
try 0 "i = 5; while (i > 0) i = i - 1; return i;"
# for
try 6 "a = 1; for (i = 0; i < 5; i = i + 1) a = a + 1; return a;"
# ステップ13: ブロック
try 21 "a = 5; if (2 > 1) { a = 20; a = a + 1; } else { a = 30; a = a - 1; } return a;"
try 29 "a = 5; if (2 < 1) { a = 20; a = a + 1; } else { a = 30; a = a - 1; } return a;"
ステップ14: 関数の呼び出しに対応する
try 5 "return test1();"
try 12 "return test2(3, 4, 5);"
try 28 "return test3(1, 2, 3, 4, 5, 6, 7);"

echo OK
