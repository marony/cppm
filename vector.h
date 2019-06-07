#pragma once

class SymbolInfo;
class Token;
class Node;

// 可変長ベクタ
class Vector {
public:
  // コンストラクタ
  Vector();

  void push(void *elem);

  // getter
  void *get(int index);
  int len() { return _len; }

private:
  void **_data;
  int _capacity;
  int _len;
};

// 可変長ベクタ
class SymbolVector {
public:
  // コンストラクタ
  SymbolVector();

  void push(SymbolInfo *elem);

  // getter
  SymbolInfo *get(int index);
  int len() { return _len; }

private:
  SymbolInfo **_data;
  int _capacity;
  int _len;
};

// 可変長ベクタ
class TokenVector {
public:
  // コンストラクタ
  TokenVector();

  void push(Token *elem);

  // getter
  Token *get(int index);
  int len() { return _len; }

private:
  Token **_data;
  int _capacity;
  int _len;
};

// 可変長ベクタ
class NodeVector {
public:
  // コンストラクタ
  NodeVector();

  void push(Node *elem);

  // getter
  Node *get(int index);
  int len() { return _len; }

private:
  Node **_data;
  int _capacity;
  int _len;
};

