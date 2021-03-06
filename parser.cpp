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

Token::Token(int ty, char* name, char *input)
  : _ty(ty), _name(name), _input(input) {
}

void Token::ty_print(int n) {
  switch (ty()) {
    default:
      debug("%*s%c: ", n, "", ty());
      break;
    case TK_NUM:
      debug("%*sNum: %d", n, "", val());
      break;
    case TK_IDENT:
      debug("%*sIdent: %s", n, "", name());
      break;
    case TK_IF:
      debug("%*sIf: ", n, "");
      break;
    case TK_ELSE:
      debug("%*sElse: ", n, "");
      break;
    case TK_WHILE:
      debug("%*sWhile: ", n, "");
      break;
    case TK_FOR:
      debug("%*sFor: ", n, "");
      break;
    case TK_RETURN:
      debug("%*sReturn: ", n, "");
      break;
    case TK_TYPE:
      debug("%*sType: %s", n, "", name());
      break;
    case TK_EQ:
      debug("%*sEQ: ==", n, "");
      break;
    case TK_NE:
      debug("%*sNE: !=", n, "");
      break;
    case TK_LE:
      debug("%*sLE: <=", n, "");
      break;
    case TK_GE:
      debug("%*sGE: >=", n, "");
      break;
    case TK_EOF:
      debug("%*sEOF: ", n, "");
      break;
  }
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

void Node::nd_print(int n) {
  switch (ty()) {
    default:
      debug("%*s%c(%d): ", n, "", ty(), ty());
      if (name() != NULL)
        debug(name());
      if (lhs() != NULL)
        lhs()->nd_print(n + 4);
      if (rhs() != NULL)
        rhs()->nd_print(n + 4);
      if (node1() != NULL)
        node1()->nd_print(n + 4);
      if (node2() != NULL)
        node2()->nd_print(n + 4);
      if (nodes() != NULL) {
        for (int i = 0; i < nodes()->len(); ++i)
          nodes()->get(i)->nd_print(n + 4);
      }
      break;
    case ND_NUM:
      debug("%*sNum: %d", n, "", val());
      break;
    case ND_IDENT:
      debug("%*sIdent: %s, %d(%s, %d)", n, "", name(), symbol()->offset(), symbol()->type()->name(), symbol()->type()->ty());
      break;
    case ND_FCALL:
      debug("%*sFCall: %s", n, "", name());
      for (int i = 0; i < nodes()->len(); ++i)
        nodes()->get(i)->nd_print(n + 4);
      break;
    case ND_FDEFIN:
      debug("%*sFdefin: %s", n, "", name());
      for (int i = 0; i < nodes()->len(); ++i)
        nodes()->get(i)->nd_print(n + 4);
      lhs()->nd_print(n + 4);
      break;
    case ND_VDEFIN:
      debug("%*sVdefin: %s, %d", n, "", name(), symbol()->offset());
      break;
    case ND_BLOCK:
      debug("%*sBlock: ", n, "");
      for (int i = 0; i < nodes()->len(); ++i)
        nodes()->get(i)->nd_print(n + 4);
      break;
    case ND_IF:
      debug("%*sIf: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
    case ND_IFELSE:
      debug("%*sIfElse: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      node1()->nd_print(n + 4);
      break;
    case ND_WHILE:
      debug("%*sWhile: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
    case ND_FOR:
      debug("%*sFor: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      node1()->nd_print(n + 4);
      node2()->nd_print(n + 4);
      break;
    case ND_RETURN:
      debug("%*sReturn: ", n, "");
      lhs()->nd_print(n + 4);
      break;
    case ND_EQ:
      debug("%*s==: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
    case ND_NE:
      debug("%*s!=: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
    case ND_LE:
      debug("%*s<=: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
    case ND_GE:
      debug("%*s>=: ", n, "");
      lhs()->nd_print(n + 4);
      rhs()->nd_print(n + 4);
      break;
  }
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

// プログラム
void program() {
  int i = 0;
  while (tokens.get(pos)->ty() != TK_EOF) {
    code[i++] = defin();
  }
  code[i] = NULL;
}

// 関数定義
Node *_function_difinition() {
  // 戻り値型
  if (!consume(TK_TYPE))
    error_at(tokens.get(pos)->input(), "関数の型がありません");
  // 型修飾('*')
  Type *type = new Type(INT, "int", NULL);
  while (tokens.get(pos)->ty() == '*') {
    ++pos;
    type = new Type(PTR, "ptr", type);
  }
  // 関数名
  if (tokens.get(pos)->ty() != TK_IDENT)
    error_at(tokens.get(pos)->input(), "関数定義がありません");
  char *name = tokens.get(pos++)->name();
  if (!consume('('))
    error_at(tokens.get(pos)->input(), "'('ではないトークンです");
  NodeVector *nodes = new NodeVector();
  if (!consume(')')) {
    // 引数の処理
    do {
      // 引数型
      if (!consume(TK_TYPE))
        error_at(tokens.get(pos)->input(), "引数の型がありません");
      // 型修飾('*')
      Type *type = new Type(INT, "int", NULL);
      while (tokens.get(pos)->ty() == '*') {
        ++pos;
        type = new Type(PTR, "ptr", type);
      }
      // 引数名
      if (tokens.get(pos)->ty() == TK_IDENT) {
        char *name = tokens.get(pos++)->name();
        SymbolInfo *symbol = map.get(name);
        if (symbol == NULL) {
          int offset = 8 * (map.len() + 1);
          symbol = new SymbolInfo(type, name, offset);
          map.put(name, symbol);
        }
        // TODO: 引数の型と識別子表の型が違ったらエラーにしないと
        type = symbol->type();
        Node *node = new Node(ND_VDEFIN, name, symbol);
        nodes->push(node);
      }
      else {
        error_at(tokens.get(pos)->input(), "関数定義の引数がありません");
      }
    } while (consume(','));
    if (!consume(')'))
      error_at(tokens.get(pos)->input(), "')'ではないトークンです");
  }
  if (tokens.get(pos)->ty() != '{')
    error_at(tokens.get(pos)->input(), "関数定義の'{'がありません");
  Node *block = stmt();
  return new Node(ND_FDEFIN, name, nodes, block);
}

// 関数定義・変数宣言
Node *defin() {
  return _function_difinition();
}

// ブロック
Node *_block() {
  NodeVector *nodes = new NodeVector();
  while (!consume('}')) {
    Node *node = stmt();
    nodes->push(node);
  }
  return new Node(ND_BLOCK, "", nodes);
}

// if, if～else
Node *_if() {
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
}

// while
Node *_while() {
  if (!consume('('))
    error_at(tokens.get(pos)->input(), "'('ではないトークンです");
  Node *node1 = expr();
  if (!consume(')'))
    error_at(tokens.get(pos)->input(), "')'ではないトークンです");
  return new Node(ND_WHILE, node1, stmt());
}

// for
Node *_for() {
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
}

// 変数定義
Node *_variable_difinition() {
  // 型
  if (!consume(TK_TYPE))
    error_at(tokens.get(pos)->input(), "変数の型がありません");
  // 型修飾('*')
  Type *type = new Type(INT, "int", NULL);
  while (tokens.get(pos)->ty() == '*') {
    ++pos;
    type = new Type(PTR, "ptr", type);
  }
  // 変数名
  Token *token = tokens.get(pos++);
  if (token->ty() != TK_IDENT)
    error_at(tokens.get(pos)->input(), "識別子がありません");

  char *name = token->name();
  SymbolInfo *symbol = map.get(name);
  if (symbol != NULL)
    error_at(tokens.get(pos)->input(), "すでに定義されている識別子です");

  int offset = 8 * (map.len() + 1);
  symbol = new SymbolInfo(type, name, offset);
  map.put(name, symbol);

  return new Node(ND_VDEFIN, name, symbol);
}

// 文
Node *stmt() {
  Node *node;

  if (consume('{'))
    return _block();
  else if (consume(TK_RETURN))
    node = new Node(ND_RETURN, expr(), NULL);
  else if (consume(TK_IF))
    return _if();
  else if (consume(TK_WHILE))
    return _while();
  else if (consume(TK_FOR))
    return _for();
  else if (tokens.get(pos)->ty() == TK_TYPE)
    node = _variable_difinition();
  else
    node = expr();

  if (!consume(';'))
    error_at(tokens.get(pos)->input(), "';'ではないトークンです");
  return node;
}

// 式
Node *expr() {
  return assign();
}

// 代入
Node *assign() {
  Node *node = equality();
  if (consume('='))
    node = new Node('=', node, assign());
  return node;
}

// 比較
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

// 二項演算子
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

// 加算・減算
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

// 乗算・除算(項)(term)
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

// 一項演算子
Node *unary() {
  if (consume('+'))
    return term();
  else if (consume('-')) // "0 - term"
    return new Node('-', new Node(ND_NUM, 0), term());
  else if (consume('*'))
    return new Node('*', unary(), NULL);
  else if (consume('&'))
    return new Node('&', unary(), NULL);

  return term();
}

// 括弧
Node *_brackets() {
  Node *node = expr();
  if (!consume(')')) {
    error_at(tokens.get(pos)->input(),
              "開きカッコに対応する閉じカッコがありません");
  }
  return node;
}

// 識別子(変数評価・関数呼び出し)
Node *_ident() {
    // 変数評価
    char *name = tokens.get(pos++)->name();
    SymbolInfo *symbol = map.get(name);
    Type *type = NULL;
    if (symbol == NULL) {
      // 変数は定義されいなくてはならない
      if (tokens.get(pos)-> ty() != '(') {
        error_at(tokens.get(pos)->input(),
                "定義されていない識別子です");
      }
      // 関数呼び出しか定義されている変数
      int offset = 8 * (map.len() + 1);
      type = new Type(INT, "int", NULL);
      symbol = new SymbolInfo(type, name, offset);
      map.put(name, symbol);
    } else {
      type = symbol->type();
    }
    // 関数呼び出し
    if (consume('(')) {
      NodeVector *nodes = new NodeVector();
      if (!consume(')')) {
        // 引数の処理
        do {
          Node *node = expr();
          nodes->push(node);
        } while (consume(','));
        if (!consume(')')) {
          error_at(tokens.get(pos)->input(),
                  "開きカッコに対応する閉じカッコがありません");
        }
      }
      return new Node(ND_FCALL, name, nodes);
    }
    return new Node(ND_IDENT, name, symbol);
}

// 因子(factor)
Node *term() {
  // 次のトークンが'('なら、"(" expr ")"のはず
  if (consume('('))
    return _brackets();

  // 数値
  if (tokens.get(pos)->ty() == TK_NUM)
    return new Node(ND_NUM, tokens.get(pos++)->val());

  // 識別子
  if (tokens.get(pos)->ty() == '*' ||
      tokens.get(pos)->ty() == '&' ||
      tokens.get(pos)->ty() == TK_IDENT)
    return _ident();

  error_at(tokens.get(pos)->input(),
           "識別子でも数値でも開きカッコでもないトークンです");
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
    // 1文字 単項演算子 / 二項演算子
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '{' || *p == '}' || *p == '(' || *p == ')' ||
        *p == '<' || *p == '>' || *p == '=' || *p == ';' ||
        *p == ',' || *p == '&') {
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

    // int
    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      tokens.push(new Token(TK_TYPE, ::strndup(p, 3), p));
      p += 3;
      continue;
    }

    // 識別子
    if (is_alpha(*p)) {
      char *q = p + 1;
      while (is_alnum(*q)) {
        ++q;
      }
      tokens.push(new Token(TK_IDENT, ::strndup(p, q - p), p));
      p = q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens.push(new Token(TK_EOF, p));
  pos = 0;
}
