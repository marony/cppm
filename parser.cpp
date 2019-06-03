#include <iostream>
#include <string.h>

#include "cppm.h"
#include "parser.h"
#include "vector.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
int pos;
Vector tokens;
Map map;
Node* code[100];

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

int is_alpha(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         (c == '_');
}

// コンストラクタ
Token::Token(int ty, char *input)
  : _ty(ty), _val(0), _input(input) {
}

Token::Token(int ty, int val, char *input)
  : _ty(ty), _val(val), _input(input) {
}

Token::Token(char* name, char *input)
  : _ty(TK_IDENT), _name(name), _input(input) {
}

// コンストラクタ
Node::Node(int ty, Node *lhs, Node *rhs)
  : _ty(ty), _lhs(lhs), _rhs(rhs) {
}

Node::Node(int val)
  : _ty(ND_NUM), _val(val) {
}

Node::Node(char *name, int offset)
  : _ty(ND_IDENT), _name(name), _offset(offset) {
}

Node::Node(int ty, Node *lhs, Node *rhs, Node *node1)
  : _ty(ty), _lhs(lhs), _rhs(rhs), _node1(node1) {
}

Node::Node(int ty, Node *lhs, Node *rhs, Node *node1, Node *node2)
  : _ty(ty), _lhs(lhs), _rhs(rhs), _node1(node1), _node2(node2) {
}

int consume(int ty) {
  if (((Token*)tokens.get(pos))->ty() != ty)
    return 0;
  ++pos;
  return 1;
}

// パーサー
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *num();

void program() {
  int i = 0;
  while (((Token*)tokens.get(pos))->ty() != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume(TK_RETURN)) {
    node = new Node(ND_RETURN, expr(), NULL);
  } else if (consume(TK_IF)) {
    if (!consume('('))
      error_at(((Token*)tokens.get(pos))->input(), "'('ではないトークンです");
    Node *node1 = expr();
    if (!consume(')'))
      error_at(((Token*)tokens.get(pos))->input(), "')'ではないトークンです");
    Node *node2 = stmt();
    if (!consume(TK_ELSE)) {
      // if
      return new Node(ND_IF, node1, node2);
    } else {
      // if else
      return new Node(ND_IFELSE, node1, node2, stmt());
    }
  } else if (consume(TK_WHILE)) {
    if (!consume('('))
      error_at(((Token*)tokens.get(pos))->input(), "'('ではないトークンです");
    Node *node1 = expr();
    if (!consume(')'))
      error_at(((Token*)tokens.get(pos))->input(), "')'ではないトークンです");
    return new Node(ND_WHILE, node1, stmt());
  } else if (consume(TK_FOR)) {
    if (!consume('('))
      error_at(((Token*)tokens.get(pos))->input(), "'('ではないトークンです");
    Node *node1 = NULL;
    Node *node2 = NULL;
    Node *node3 = NULL;
    if (!consume(';')) {
      node1 = expr();
      if (!consume(';'))
        error_at(((Token*)tokens.get(pos))->input(), "';'ではないトークンです");
    }
    if (!consume(';')) {
      node2 = expr();
      if (!consume(';'))
        error_at(((Token*)tokens.get(pos))->input(), "';'ではないトークンです");
    }
    if (!consume(';')) {
      node3 = expr();
      if (!consume(';'))
        error_at(((Token*)tokens.get(pos))->input(), "';'ではないトークンです");
    }
    if (!consume(')'))
      error_at(((Token*)tokens.get(pos))->input(), "')'ではないトークンです");
    return new Node(ND_FOR, node1, node2, node3, stmt());
  } else {
    node = expr();
  }
  
  if (!consume(';'))
    error_at(((Token*)tokens.get(pos))->input(), "';'ではないトークンです");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume('='))
    node = new Node('=', node, assign());
  return node;
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

  // 識別子
  if (((Token*)tokens.get(pos))->ty() == TK_IDENT)
  {
    char *name = ((Token*)tokens.get(pos))->name();
    void *id = map.get(name);
    if (id == NULL) {
      id = (void*)(8 * (map.len() + 1));
      map.put(name, id);
    }
    return new Node(((Token*)tokens.get(pos++))->name(), (int)(long)id);
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
        *p == '(' || *p == ')' || *p == '<' || *p == '>' ||
        *p == '=' || *p == ';') {
      tokens.push(new Token(*p, p));
      ++p;
      continue;
    }

    // 整数
    if (isdigit(*p)) {
      tokens.push(new Token(TK_NUM, std::strtol(p, &p, 10), p));
      continue;
    }

    // return
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      tokens.push(new Token(TK_RETURN, p));
      p += 6;
      continue;
    }

    // if
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      tokens.push(new Token(TK_IF, p));
      p += 2;
      continue;
    }

    // else
    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      tokens.push(new Token(TK_ELSE, p));
      p += 4;
      continue;
    }

    // while
    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      tokens.push(new Token(TK_WHILE, p));
      p += 5;
      continue;
    }

    // for
    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      tokens.push(new Token(TK_FOR, p));
      p += 3;
      continue;
    }

    // 識別子
    if (is_alpha(*p)) {
      char *q = p + 1;
      while (is_alnum(*q)) {
        ++q;
      }
      tokens.push(new Token(::strndup(p, q - p), p));
      p = q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens.push(new Token(TK_EOF, p));
  pos = 0;
}
