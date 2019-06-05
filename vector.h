#pragma once

// 可変長ベクタ
class Vector {
public:
  // コンストラクタ
  Vector();

  void push(void *elem);

  // getter
  void *get(int index);
  int len() { return _len; }

private:
  void **_data;
  int _capacity;
  int _len;
};

