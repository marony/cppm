#pragma once

#include "vector.h"
#include "map.h"

/*
# EBNF
program    = difin*
difin      = FIXME: func(param, paaram...) { stmt* }
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? term
term       = num
           | ident ("(" ")")?
           | "(" expr ")"
*/

/*
# 演算子の優先順位
    == !=
    < <= > >=
    + -
    * /
    単項+ 単項-
    ()
*/

// トークンの型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  TK_NUM = 256, // 整数トークン
  TK_IDENT,     // 識別子
  TK_IF,        // if
  TK_ELSE,      // else
  TK_WHILE,     // while
  TK_FOR,       // for
  TK_RETURN,    // return
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LE,        // <=
  TK_GE,        // >=
  TK_EOF,       // 入力の終わりを表すトークン

  TK_INT,
};

// トークンの型
class Token {
public:
  // コンストラクタ
  Token(int ty, char *input);
  Token(int ty, int val, char *input);
  Token(char* name, char *input);

  // getter
  int ty() { return _ty; }
  int val() { return _val; }
  char *name() { return _name; }
  char *input() { return _input; }

private:
  int _ty;       // トークンの型
  int _val;      // tyがTK_NUMの場合、その数値
  char *_name;   // tyがTK_IDENTの場合、その名前
  char *_input;  // トークン文字列（エラーメッセージ用）
};

// 抽象構文木の型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  ND_NUM = 256, // 整数
  ND_IDENT,     // 識別子
  ND_FCALL,     // 関数呼び出し
  ND_FDEFIN,    // 関数定義
  ND_BLOCK,     // ブロック
  ND_IF,        // if
  ND_IFELSE,    // if else
  ND_WHILE,     // while
  ND_FOR,       // for
  ND_RETURN,    // return
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LE,        // <=
  ND_GE,        // >=
};

class Type;

// 抽象構文木
class Node {
public:
  // コンストラクタ
  Node(int ty, Node *lhs, Node *rhs);
  Node(int ty, int val);
  Node(int ty, char* name, SymbolInfo *symbol);
  Node(int ty, Node *lhs, Node *rhs, Node *node1);
  Node(int ty, Node *lhs, Node *rhs, Node *node1, Node *node2);
  Node(int ty, char *name, NodeVector *nodes);
  Node(int ty, char *name, NodeVector *nodes, Node *node);

  // getter
  int ty() { return _ty; }
  Node *lhs() { return _lhs; }
  Node *rhs() { return _rhs; }
  int val() { return _val; }
  char *name() { return _name; }
  SymbolInfo *symbol() { return _symbol; }
  Node *node1() { return _node1; }
  Node *node2() { return _node2; }
  NodeVector *nodes() { return _nodes; }

private:
  int _ty;                // 演算子かND_NUM
  Node *_lhs;             // 左辺
  Node *_rhs;             // 右辺
  int _val;               // tyがND_NUMの場合のみ使う
  char *_name;            // tyがND_IDENTの場合のみ使う
  SymbolInfo *_symbol;    // tyがND_IDENTの場合のみ使う
  Node *_node1;           // tyがND_IFELSE, ND_FORの場合のみ使う
  Node *_node2;           // tyがND_FORの場合のみ使う
  NodeVector *_nodes;     // tyがND_BLOCKの場合、ブロックの中のstmts
};

extern void tokenize(char *p);
extern void program();
extern Node* code[];
extern SymbolMap map;
