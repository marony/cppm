#include <cstdlib>

#include "vector.h"

// コンストラクタ
Vector::Vector()
  : _data((void**)::malloc(sizeof(void*) * 16)), _capacity(16), _len(0) {
}

void Vector::push(void *elem) {
  if (_capacity == _len) {
    _capacity *= 2;
    _data = (void**)::realloc(_data, sizeof(void*) * _capacity);
  }
  _data[_len++] = elem;
}
