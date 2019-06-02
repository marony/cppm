#!/bin/bash

try() {
  expected="$1"
  input="$2"

  ./cppm "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
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

echo OK
