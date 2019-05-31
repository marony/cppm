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
try 0 0
try 42 42
# ステップ2：加減算のできるコンパイラの作成
try 21 '5+20-4'
# ステップ3：トークナイザを導入
try 41 " 12 + 34 - 5 "
# 生成規則による演算子の優先順位の表現
try 14 "1 * 2 + 3 * 4"
# ステップ4：四則演算のできる言語の作成
try 47 "5+6*7"
try 15 "5*(9-6)"
try 4 "(3+5)/2"

echo OK
