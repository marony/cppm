#pragma once

#include "vector.h"

class Map {
public:
  void put(char *key, void *val);
  void *get(int i);
  void *get(char *key);

  // getter
  int len() { return keys.len(); }

private:
  Vector keys;
  Vector vals;
};
