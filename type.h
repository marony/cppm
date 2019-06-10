#pragma once

enum {
  INT,
  PTR,
};

// 型情報
class Type {
public:
  Type(int ty, Type *type);

private:
  int _ty;
  Type *_type;
};

// 識別子の情報
class SymbolInfo {
public:
  SymbolInfo(Type *type, int offset);

  Type *type() { return _type; }
  int offset() { return _offset; }

private:
  Type *_type;
  int _offset;
};
