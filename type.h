#pragma once

enum {
  INT,
  PTR,
};

// 型情報
class Type {
public:
  Type(int ty, char *name, Type *type);

  int ty() { return _ty; }
  char *name() { return _name; }
  
private:
  int _ty;
  char *_name;
  Type *_type;
};

// 識別子の情報
class SymbolInfo {
public:
  SymbolInfo(Type *type, char *name, int offset);

  Type *type() { return _type; }
  char *name() { return _name; }
  int offset() { return _offset; }

private:
  Type *_type;
  char *_name;
  int _offset;
};
