#pragma once

#include "vector.h"

class Type;

class SymbolMap {
public:
  void put(char *key, SymbolInfo *val);
  SymbolInfo *get(int i);
  SymbolInfo *get(char *key);

  // getter
  int len() { return keys.len(); }

private:
  Vector keys;
  SymbolVector vals;
};
