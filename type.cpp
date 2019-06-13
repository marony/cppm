#include "type.h"

Type::Type(int ty, char *name, Type *type)
  : _ty(ty), _name(name), _type(type) {
}

SymbolInfo::SymbolInfo(Type *type, char *name, int offset)
  : _type(type), _name(name), _offset(offset) {
}
