#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <string.h>

#include "parser.h"
#include "vector.h"
#include "map.h"
#include "codegen.h"

#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wwrite-strings"

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

// デバッグ出力
// printfと同じ引数を取る
void debug(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
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
  // Vectorテスト
  Vector *vec = new Vector();
  expect(__LINE__, 0, vec->len());

  for (int i = 0; i < 100; i++)
    vec->push((void *)i);

  expect(__LINE__, 100, vec->len());
  expect(__LINE__, 0, (long)vec->get(0));
  expect(__LINE__, 50, (long)vec->get(50));
  expect(__LINE__, 99, (long)vec->get(99));

  // Mapテスト
  Map *map = new Map();
  expect(__LINE__, 0, (long)map->get("foo"));

  map->put("foo", (void *)2);
  expect(__LINE__, 2, (long)map->get("foo"));

  map->put("bar", (void *)4);
  expect(__LINE__, 4, (long)map->get("bar"));

  map->put("foo", (void *)6);
  expect(__LINE__, 6, (long)map->get("foo"));

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
  // 結果はcodeに保存される
  tokenize(argv[1]);
  program();

  // アセンブリの前半部分を出力
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".global main" << std::endl;
  std::cout << "main:" << std::endl;

  // プロローグ
  // 変数26個分の領域を確保する
  std::cout << "  push rbp" << std::endl;
  std::cout << "  mov rbp, rsp" << std::endl;
  int identSize = map.len() * 8;
  std::cout << "  sub rsp, " << identSize << std::endl;

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; ++i) {
    debug("%d\n", code[i]->ty());
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    std::cout << "  pop rax" << std::endl;
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  std::cout << "  mov rsp, rbp" << std::endl;
  std::cout << "  pop rbp" << std::endl;
  std::cout << "  ret" << std::endl;
  
  return 0;
}
