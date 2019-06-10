#include "type.h"

Type::Type(int ty, Type *type)
  : _ty(ty), _type(type) {
}

SymbolInfo::SymbolInfo(Type *type, int offset)
  : _type(type), _offset(offset) {
}
