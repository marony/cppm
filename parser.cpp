#include <iostream>
#include <string.h>

#include "parser.h"
#include "cppm.h"
#include "type.h"
#include "debug.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
int pos;
TokenVector tokens;
SymbolMap map;
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

Node::Node(int ty, int val)
  : _ty(ty), _val(val) {
}

Node::Node(int ty, char *name, SymbolInfo *symbol)
  : _ty(ty), _name(name), _symbol(symbol) {
}

Node::Node(int ty, Node *lhs, Node *rhs, Node *node1)
  : _ty(ty), _lhs(lhs), _rhs(rhs), _node1(node1) {
}

Node::Node(int ty, Node *lhs, Node *rhs, Node *node1, Node *node2)
  : _ty(ty), _lhs(lhs), _rhs(rhs), _node1(node1), _node2(node2) {
}

Node::Node(int ty, char *name, NodeVector *nodes)
  : _ty(ty), _name(name), _nodes(nodes) {
}

Node::Node(int ty, char *name, NodeVector *nodes, Node *node)
  : _ty(ty), _name(name), _nodes(nodes), _lhs(node) {
}

int consume(int ty) {
  if (tokens.get(pos)->ty() != ty)
    return 0;
  ++pos;
  return 1;
}

// パーサー
Node *defin();
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
  while (tokens.get(pos)->ty() != TK_EOF) {
    code[i++] = defin();
  }
  code[i] = NULL;
}

Node *defin() {
  debug("defin: %d", tokens.get(pos)->ty());
  if (!consume(TK_IDENT))
    error_at(tokens.get(pos)->input(), "関数定義がありません");
  // 関数定義
  char *name = tokens.get(pos - 1)->name();
  if (!consume('('))
    error_at(tokens.get(pos)->input(), "'('ではないトークンです");
  NodeVector *nodes = new NodeVector();
  if (!consume(')')) {
    // 引数の処理
    do {
      if (tokens.get(pos)->ty() == TK_IDENT) {
        char *name = tokens.get(pos++)->name();
        SymbolInfo *symbol = map.get(name);
        Type *type = NULL;
        if (symbol == NULL) {
          int offset = 8 * (map.len() + 1);
          type = new Type(INT);
          symbol = new SymbolInfo(type, offset);
          map.put(name, symbol);
        }
        type = symbol->type();
        Node *node = new Node(ND_IDENT, name, symbol);
        nodes->push(node);
      }
      else
        error_at(tokens.get(pos)->input(),
                "関数定義の引数がありません");
    } while (consume(','));
    if (!consume(')'))
      error_at(tokens.get(pos)->input(), "')'ではないトークンです");
  }
  if (tokens.get(pos)->ty() != '{')
    error_at(tokens.get(pos)->input(), "関数定義の'{'がありません");
  Node *block = stmt();
  return new Node(ND_FDEFIN, name, nodes, block);
}

Node *stmt() {
  debug("stmt: %d", tokens.get(pos)->ty());
  Node *node;

  if (consume('{')) {
    // ブロック
    NodeVector *nodes = new NodeVector();
    while (!consume('}')) {
      Node *node = stmt();
      nodes->push(node);
    }
    return new Node(ND_BLOCK, "", nodes);
  } else if (consume(TK_RETURN)) {
    // return
    node = new Node(ND_RETURN, expr(), NULL);
  } else if (consume(TK_IF)) {
    // if
    if (!consume('('))
      error_at(tokens.get(pos)->input(), "'('ではないトークンです");
    Node *node1 = expr();
    if (!consume(')'))
      error_at(tokens.get(pos)->input(), "')'ではないトークンです");
    Node *node2 = stmt();
    if (!consume(TK_ELSE)) {
      // if
      return new Node(ND_IF, node1, node2);
    } else {
      // if else
      return new Node(ND_IFELSE, node1, node2, stmt());
    }
  } else if (consume(TK_WHILE)) {
    // while
    if (!consume('('))
      error_at(tokens.get(pos)->input(), "'('ではないトークンです");
    Node *node1 = expr();
    if (!consume(')'))
      error_at(tokens.get(pos)->input(), "')'ではないトークンです");
    return new Node(ND_WHILE, node1, stmt());
  } else if (consume(TK_FOR)) {
    // for
    if (!consume('('))
      error_at(tokens.get(pos)->input(), "'('ではないトークンです");
    Node *node1 = NULL;
    Node *node2 = NULL;
    Node *node3 = NULL;
    if (!consume(';')) {
      node1 = expr();
      if (!consume(';'))
        error_at(tokens.get(pos)->input(), "';'ではないトークンです");
    }
    if (!consume(';')) {
      node2 = expr();
      if (!consume(';'))
        error_at(tokens.get(pos)->input(), "';'ではないトークンです");
    }
    if (!consume(')')) {
      node3 = expr();
    }
    if (!consume(')'))
      error_at(tokens.get(pos)->input(), "')'ではないトークンです");
    return new Node(ND_FOR, node1, node2, node3, stmt());
  } else {
    // expr
    node = expr();
  }
  
  if (!consume(';'))
    error_at(tokens.get(pos)->input(), "';'ではないトークンです");
  return node;
}

Node *expr() {
  debug("expr: %d", tokens.get(pos)->ty());
  return assign();
}

Node *assign() {
  debug("assign: %d", tokens.get(pos)->ty());
  Node *node = equality();
  if (consume('='))
    node = new Node('=', node, assign());
  return node;
}

Node *equality() {
  debug("equality: %d", tokens.get(pos)->ty());
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
  debug("relational: %d", tokens.get(pos)->ty());
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
  debug("add: %d", tokens.get(pos)->ty());
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
  debug("mul: %d", tokens.get(pos)->ty());
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
  debug("unary: %d", tokens.get(pos)->ty());
  if (consume('+'))
    return term();
  if (consume('-')) // "0 - term"
    return new Node('-', new Node(ND_NUM, 0), term());
  return term();
}

Node *term() {
  debug("term: %d", tokens.get(pos)->ty());
  // 次のトークンが'('なら、"(" expr ")"のはず
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(tokens.get(pos)->input(),
               "開きカッコに対応する閉じカッコがありません");
    return node;
  }

  // 識別子
  if (tokens.get(pos)->ty() == TK_IDENT)
  {
    char *name = tokens.get(pos++)->name();
    debug("map address 2 = %d", &map);
    SymbolInfo *symbol = map.get(name);
    Type *type = NULL;
    if (symbol == NULL) {
      int offset = 8 * (map.len() + 1);
      type = new Type(INT);
      symbol = new SymbolInfo(type, offset);
      map.put(name, symbol);
    }
    type = symbol->type();
    debug("map length = %d", map.len());
    // 関数呼び出し
    if (consume('(')) {
      NodeVector *nodes = new NodeVector();
      if (!consume(')')) {
        // 引数の処理
        do {
          Node *node = expr();
          nodes->push(node);
        } while (consume(','));
        if (!consume(')'))
          error_at(tokens.get(pos)->input(),
                  "開きカッコに対応する閉じカッコがありません");
      }
      return new Node(ND_FCALL, name, nodes);
    }
    return new Node(ND_IDENT, name, symbol);
  }

  // そうでなければ数値のはず
  if (tokens.get(pos)->ty() == TK_NUM)
    return new Node(ND_NUM, tokens.get(pos++)->val());

  debug("%d\n", tokens.get(pos)->ty());
  error_at(tokens.get(pos)->input(),
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
        *p == '{' || *p == '}' || *p == '(' || *p == ')' ||
        *p == '<' || *p == '>' || *p == '=' || *p == ';' ||
        *p == ',') {
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
