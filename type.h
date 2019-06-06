#pragma once

class Type {
public:
  Type(int offset);

  int offset() { return _offset; }

private:
  int _offset;
};
