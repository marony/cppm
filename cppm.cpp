#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <string.h>

#include "parser.h"
#include "vector.h"
#include "codegen.h"

#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

// 入力プログラム
char *user_input;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// テスト
void expect(int line, int expected, int actual) {
  if (expected == actual)
    return;
  fprintf(stderr, "%d: %d expected, but got %d\n",
          line, expected, actual);
  exit(1);
}

void runtest() {
  Vector *vec = new Vector();
  expect(__LINE__, 0, vec->len());

  for (int i = 0; i < 100; i++)
    vec->push((void *)i);

  expect(__LINE__, 100, vec->len());
  expect(__LINE__, 0, (long)vec->get(0));
  expect(__LINE__, 50, (long)vec->get(50));
  expect(__LINE__, 99, (long)vec->get(99));

  printf("OK\n");
}

// まいんちゃん
int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "引数の個数が正しくありません\n";
    return 1;
  }

  if (::strcmp(argv[1], "-test") == 0) {
    // テストの実行
    runtest();
    return 0;
  }

  // トークナイズしてパースする
  tokenize(argv[1]);
  Node *node = expr();

  // アセンブリの前半部分を出力
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".global main" << std::endl;
  std::cout << "main:" << std::endl;

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  std::cout << "  pop rax" << std::endl;
  std::cout << "  ret" << std::endl;

  return 0;
}
