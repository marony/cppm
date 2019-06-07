#include "type.h"

Type::Type(int ty)
  : _ty(ty) {
}

SymbolInfo::SymbolInfo(Type *type, int offset)
  : _type(type), _offset(offset) {
}
