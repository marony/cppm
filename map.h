#pragma once

#include "vector.h"

class Type;

class SymbolMap {
public:
  void put(char *key, Type *val);
  Type *get(int i);
  Type *get(char *key);

  // getter
  int len() { return keys.len(); }

private:
  Vector keys;
  TypeVector vals;
};
