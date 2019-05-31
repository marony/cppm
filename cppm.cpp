#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

/*
# EBNF
expr = mul ("+" mul | "-" mul)*
mul  = term ("*" term | "/" term)*
term = num | "(" expr ")"
*/

// トークンの型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  TK_NUM = 256, // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
};

// トークンの型
class Token {
public:
  // コンストラクタ
  Token(int ty, char *input);
  Token(int ty, int val, char *input);

  // getter
  int ty() { return _ty; }
  int val() { return _val; }
  char *input() { return _input; }

private:
  int _ty;      // トークンの型
  int _val;     // tyがTK_NUMの場合、その数値
  char *_input; // トークン文字列（エラーメッセージ用）
};

// コンストラクタ
Token::Token(int ty, char *input)
  : _ty(ty), _val(0), _input(input) {
}

Token::Token(int ty, int val, char *input)
  : _ty(ty), _val(val), _input(input) {
}

// 入力プログラム
char *user_input;

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
int pos;
Token *tokens[100];

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
};

// コンストラクタ
Node::Node(int ty, Node *lhs, Node *rhs)
  : _ty(ty), _lhs(lhs), _rhs(rhs) {
}

Node::Node(int val)
  : _ty(ND_NUM), _val(val) {
}

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
  if (tokens[pos]->ty() != ty)
    return 0;
  ++pos;
  return 1;
}

// パーサー
Node *expr();
Node *mul();
Node *term();
Node *num();

Node *expr() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new Node('+', node, mul());
    else if (consume('-'))
      node = new Node('-', node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = term();

  for (;;) {
    if (consume('*'))
      node = new Node('*', node, term());
    else if (consume('/'))
      node = new Node('/', node, term());
    else
      return node;
  }
}

Node *term() {
  // 次のトークンが'('なら、"(" expr ")"のはず
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(tokens[pos]->input(),
               "開きカッコに対応する閉じカッコがありません");
    return node;
  }

  // そうでなければ数値のはず
  if (tokens[pos]->ty() == TK_NUM)
    return new Node(tokens[pos++]->val());

  error_at(tokens[pos]->input(),
           "数値でも開きカッコでもないトークンです");
}

Node *num() {
  if (tokens[pos]->ty() == TK_NUM)
    return new Node(tokens[pos++]->val());

  error_at(tokens[pos]->input(), "数値ではないトークンです");
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
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
      tokens[i] = new Token(*p, p);
      ++i;
      ++p;
      continue;
    }

    // 整数
    if (isdigit(*p)) {
      tokens[i] = new Token(TK_NUM, std::strtol(p, &p, 10), p);
      ++i;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens[i] = new Token(TK_EOF, p);
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
  case '*':
    printf("  imul rdi\n");
    break;
  case '/':
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }

  std::cout << "  push rax" << std::endl;
}

// まいんちゃん
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
