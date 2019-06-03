#include "map.h"
#include <cstring>

void Map::put(char *key, void *val) {
  keys.push(key);
  vals.push(val);
}

void *Map::get(char *key) {
  for (int i = keys.len() - 1; i >= 0; --i) {
    if (std::strcmp((const char*)keys.get(i), key) == 0)
      return vals.get(i);
  }
  return NULL;
}
