#pragma once

// 可変長ベクタ
class Vector {
public:
  // コンストラクタ
  Vector();

  // NOTE: デストラクタを作ったらコピーコンストラクタも作ること

  void push(void *elem);

  // getter
  void *get(int i) { return _data[i]; }
  int len() { return _len; }

private:
  void **_data;
  int _capacity;
  int _len;
};

