#include "map.h"
#include <cstring>

void SymbolMap::put(char *key, SymbolInfo *val) {
  keys.push(key);
  vals.push(val);
}

SymbolInfo *SymbolMap::get(int index) {
  if (index < vals.len())
    return vals.get(index);
  return NULL;
}

SymbolInfo *SymbolMap::get(char *key) {
  for (int i = keys.len() - 1; i >= 0; --i) {
    if (std::strcmp((const char*)keys.get(i), key) == 0)
      return vals.get(i);
  }
  return NULL;
}
