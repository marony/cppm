#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

/*
# EBNF
expr = num ("+" num | "-" num)*
*/

// トークンの型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  TK_NUM = 256, // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
  int ty;      // トークンの型
  int val;     // tyがTK_NUMの場合、その数値
  char *input; // トークン文字列（エラーメッセージ用）
} Token;

// 抽象構文木の型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  ND_NUM = 256, // 整数トークン
};

// 抽象構文木
class Node {
public:
  // コンストラクタ
  Node(int ty, Node *lhs, Node *rhs);
  Node(int val);

  // getter
  int ty() { return _ty; }
  Node *lhs() { return _lhs; }
  Node *rhs() { return _rhs; }
  int val() { return _val; }

private:
  int _ty;    // 演算子かND_NUM
  Node *_lhs; // 左辺
  Node *_rhs; // 右辺
  int _val;   // tyがND_NUMの場合のみ使う

public:

};

// コンストラクタ
Node::Node(int ty, Node *lhs, Node *rhs) {
  _ty = ty;
  _lhs = lhs;
  _rhs = rhs;
}

Node::Node(int val) {
  _ty = ND_NUM;
  _val = val;
}

// 入力プログラム
char *user_input;

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
int pos;
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

int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  ++pos;
  return 1;
}

// パーサー
Node *num() {
  if (tokens[pos].ty == TK_NUM)
    return new Node(tokens[pos++].val);

  error_at(tokens[pos].input, "数値ではないトークンです");
}

Node *expr() {
  Node *node = num();

  for (;;) {
    if (consume('+'))
      node = new Node('+', node, num());
    else if (consume('-'))
      node = new Node('-', node, num());
    else
      return node;
  }
}

// コード生成
void gen(Node *node) {
  if (node->ty() == ND_NUM) {
    std::cout << "  push " << node->val() << std::endl;
    return;
  }

  // 左辺の生成
  gen(node->lhs());
  // 右辺の生成
  gen(node->rhs());

  // 右辺
  std::cout << "  pop rdi" << std::endl;
  // 左辺
  std::cout << "  pop rax" << std::endl;

  // 演算の生成
  switch (node->ty()) {
  case '+':
    std::cout << "  add rax, rdi" << std::endl;
    break;
  case '-':
    std::cout << "  sub rax, rdi" << std::endl;
    break;
  }

  std::cout << "  push rax" << std::endl;
}

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize(char *p) {
  user_input = p;

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
      tokens[i].val = std::strtol(p, &p, 10);
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

  // トークナイズしてパースする
  tokenize(argv[1]);
  Node *node = expr();

  // アセンブリの前半部分を出力
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".global main" << std::endl;
  std::cout << "main:" << std::endl;

  // 抽象構文木を下りながらコード生成
  pos = 0;
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  std::cout << "  pop rax" << std::endl;
  std::cout << "  ret" << std::endl;

  return 0;
}
