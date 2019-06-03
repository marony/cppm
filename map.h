#include "vector.h"

class Map {
public:
  void put(char *key, void *val);
  void *get(char *key);

private:
  Vector keys;
  Vector vals;
};
