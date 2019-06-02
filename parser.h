#pragma once

/*
# EBNF
expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? term
term       = num | "(" expr ")"
*/

/*
program    = stmt*
stmt       = expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? term
term       = num | ident | "(" expr ")"
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
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LE,        // <=
  TK_GE,        // >=
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

// 抽象構文木の型を表す値
// 1文字の演算子はその演算子そのものを値とする
enum {
  ND_NUM = 256, // 整数
  ND_IDENT,     // 識別子
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LE,        // <=
  ND_GE,        // >=
};

// 抽象構文木
class Node {
public:
  // コンストラクタ
  Node(int ty, Node *lhs, Node *rhs);
  Node(int val);
  Node(char name);

  // getter
  int ty() { return _ty; }
  Node *lhs() { return _lhs; }
  Node *rhs() { return _rhs; }
  int val() { return _val; }
  char name() { return _name; }

private:
  int _ty;    // 演算子かND_NUM
  Node *_lhs; // 左辺
  Node *_rhs; // 右辺
  int _val;   // tyがND_NUMの場合のみ使う
  char _name; // tyがND_IDENTの場合のみ使う
};

extern void tokenize(char *p);
extern void program();
extern Node* code[];
