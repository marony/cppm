#include <cstdlib>
#include <cstring>

#include "vector.h"
#include "debug.h"

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

void *Vector::get(int index) {
  if (index < _len)
    return _data[index];
  return NULL;
}

// コンストラクタ
TypeVector::TypeVector()
  : _data((Type**)::malloc(sizeof(Type*) * 16)), _capacity(16), _len(0) {
}

void TypeVector::push(Type *elem) {
  if (_capacity == _len) {
    _capacity *= 2;
    _data = (Type**)::realloc(_data, sizeof(Type*) * _capacity);
  }
  _data[_len++] = elem;
}

Type *TypeVector::get(int index) {
  if (index < _len)
    return _data[index];
  return NULL;
}

// コンストラクタ
TokenVector::TokenVector()
  : _data((Token**)::malloc(sizeof(Token*) * 16)), _capacity(16), _len(0) {
}

void TokenVector::push(Token *elem) {
  if (_capacity == _len) {
    _capacity *= 2;
    _data = (Token**)::realloc(_data, sizeof(Token*) * _capacity);
  }
  _data[_len++] = elem;
}

Token *TokenVector::get(int index) {
  if (index < _len)
    return _data[index];
  return NULL;
}

// コンストラクタ
NodeVector::NodeVector()
  : _data((Node**)::malloc(sizeof(Node*) * 16)), _capacity(16), _len(0) {
}

void NodeVector::push(Node *elem) {
  if (_capacity == _len) {
    _capacity *= 2;
    _data = (Node**)::realloc(_data, sizeof(Node*) * _capacity);
  }
  _data[_len++] = elem;
}

Node *NodeVector::get(int index) {
  if (index < _len)
    return _data[index];
  return NULL;
}
