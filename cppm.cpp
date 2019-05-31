#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

// トークンの型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  TK_NUM = 256, // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
  int ty;      // トークンの型
  int val;     // tyがTK_NUMの場合、その数値
  char *input; // トークン文字列（エラーメッセージ用）
} Token;

// 入力プログラム
char *user_input;

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];

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

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize() {
  char *p = user_input;

  int i = 0;
  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      ++p;
      continue;
    }

    // '+', '-' 二項演算子
    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      ++i;
      ++p;
      continue;
    }

    // 整数
    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      ++i;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "引数の個数が正しくありません\n";
    return 1;
  }

  // トークナイズする
  user_input = argv[1];
  tokenize();

  // アセンブリの前半部分を出力
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".global main" << std::endl;
  std::cout << "main:" << std::endl;

  // 式の最初は数でなければならないので、それをチェックして
  // 最初のmov命令を出力
  if (tokens[0].ty != TK_NUM)
    error_at(tokens[0].input, "数ではありません");
  std::cout << "  mov rax, " << tokens[0].val << std::endl;

  // `+ <数>`あるいは`- <数>`というトークンの並びを消費しつつ
  // アセンブリを出力
  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      ++i;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      std::cout << "  add rax, " << tokens[i].val << std::endl;
      ++i;
      continue;
    }

    if (tokens[i].ty == '-') {
      ++i;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      std::cout << "  sub rax, " << tokens[i].val << std::endl;
      ++i;
      continue;
    }

    error_at(tokens[i].input, "予期しないトークンです");
  }

  std::cout << "  ret" << std::endl;

  return 0;
}
