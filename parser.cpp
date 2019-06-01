#include <iostream>
#include <string.h>

#include "cppm.h"
#include "parser.h"
#include "vector.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
int pos;
Vector tokens;

// コンストラクタ
Token::Token(int ty, char *input)
  : _ty(ty), _val(0), _input(input) {
}

Token::Token(int ty, int val, char *input)
  : _ty(ty), _val(val), _input(input) {
}

// コンストラクタ
Node::Node(int ty, Node *lhs, Node *rhs)
  : _ty(ty), _lhs(lhs), _rhs(rhs) {
}

Node::Node(int val)
  : _ty(ND_NUM), _val(val) {
}

int consume(int ty) {
  if (((Token*)tokens.get(pos))->ty() != ty)
    return 0;
  ++pos;
  return 1;
}

// パーサー
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *num();

Node *expr() {
  Node *node = equality();
}

Node *equality() {
  Node *node = relational();

  for (;;) {
      if (consume(TK_EQ))
        node = new Node(ND_EQ, node, relational());
      else if (consume(TK_NE))
        node = new Node(ND_NE, node, relational());
      else
        return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
      if (consume('<'))
        node = new Node('<', node, add());
      else if (consume(TK_LE))
        node = new Node(ND_LE, node, add());
      else if (consume('>'))
        node = new Node('>', node, add());
      else if (consume(TK_GE))
        node = new Node(ND_GE, node, add());
      else
        return node;
  }
}

Node *add() {
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
  Node *node = unary();

  for (;;) {
    if (consume('*'))
      node = new Node('*', node, unary());
    else if (consume('/'))
      node = new Node('/', node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume('+'))
    return term();
  if (consume('-')) // "0 - term"
    return new Node('-', new Node(0), term());
  return term();
}

Node *term() {
  // 次のトークンが'('なら、"(" expr ")"のはず
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(((Token*)tokens.get(pos))->input(),
               "開きカッコに対応する閉じカッコがありません");
    return node;
  }

  // そうでなければ数値のはず
  if (((Token*)tokens.get(pos))->ty() == TK_NUM)
    return new Node(((Token*)tokens.get(pos++))->val());

  std::cerr << " *** " << ((Token*)tokens.get(pos))->ty() << " *** " << std::endl;
  error_at(((Token*)tokens.get(pos))->input(),
           "数値でも開きカッコでもないトークンです");
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

    // 複数文字 二項演算子
    if (::strncmp(p, "==", 2) == 0) {
      tokens.push(new Token(TK_EQ, p));
      p += 2;
      continue;
    }
    else if (::strncmp(p, "!=", 2) == 0) {
      tokens.push(new Token(TK_NE, p));
      p += 2;
      continue;
    }
    else if (::strncmp(p, "<=", 2) == 0) {
      tokens.push(new Token(TK_LE, p));
      p += 2;
      continue;
    }
    else if (::strncmp(p, ">=", 2) == 0) {
      tokens.push(new Token(TK_GE, p));
      p += 2;
      continue;
    }
    // 1文字 二項演算子
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '(' || *p == ')' || *p == '<' || *p == '>') {
      tokens.push(new Token(*p, p));
      ++p;
      continue;
    }

    // 整数
    if (isdigit(*p)) {
      tokens.push(new Token(TK_NUM, std::strtol(p, &p, 10), p));
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens.push(new Token(TK_EOF, p));
  pos = 0;
}
