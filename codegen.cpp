#include <iostream>

#include "codegen.h"
#include "cppm.h"
#include "type.h"
#include "debug.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

int labelNo = 0;

// アセンブリラベル用通番
int newLabelNo() {
  return ++labelNo;
}

// 左辺値
// 変数のアドレスをpush
void _gen_lval(Node *node) {
  if (node->ty() != ND_IDENT)
    error("代入の左辺値が変数ではありません");

  // 変数のアドレスをpush
  SymbolInfo *symbol = node->symbol();
  Type *type = symbol->type();
  int offset = symbol->offset();
  std::cout << "  mov rax, rbp" << std::endl;
  std::cout << "  sub rax, " << offset << std::endl;
  std::cout << "  push rax" << std::endl;
}

// 変数をレジスタかスタックに積む
void _gen_variable_stack(int i, Node *node) {
  char* name = node->name();
  SymbolInfo *symbol = node->symbol();
  Type *type = symbol->type();
  int offset = symbol->offset();
  std::cout << "  mov rax, rbp" << std::endl;
  std::cout << "  sub rax, " << offset << std::endl;
  switch (i) {
  case 0:
    std::cout << "  mov [rax], rdi" << std::endl;
    break;
  case 1:
    std::cout << "  mov [rax], rsi" << std::endl;
    break;
  case 2:
    std::cout << "  mov [rax], rdx" << std::endl;
    break;
  case 3:
    std::cout << "  mov [rax], rcx" << std::endl;
    break;
  case 4:
    std::cout << "  mov [rax], r8" << std::endl;
    break;
  case 5:
    std::cout << "  mov [rax], r9" << std::endl;
    break;
  default:
    std::cout << "  	mov	r10, QWORD PTR " << (i - (6 - 1)) * 16 << "[rbp]" << std::endl;
    std::cout << "  mov [rax], r10" << std::endl;
    break;
  }
}

// 関数の始めのコード
void _gen_prologue(Node *node) {
  // プロローグ
  std::cout << node->name() << ":" << std::endl;
  // rbpの保存
  std::cout << "  push rbp" << std::endl;
  // rbpの設定
  std::cout << "  mov rbp, rsp" << std::endl;
  // 変数分スタックを確保
  std::cout << "  sub rsp, " << (map.len() * 8) << std::endl;
  // 関数の引数をレジスタかスタックに積む
  for (int i = 0; i < node->nodes()->len(); ++i)
    _gen_variable_stack(i, node->nodes()->get(i));
}

// 関数の終わりのコード
void _gen_epilogue(Node *node) {
  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  // スタックを復元
  std::cout << "  mov rsp, rbp" << std::endl;
  // rbpを復元
  std::cout << "  pop rbp" << std::endl;

  std::cout << "  ret" << std::endl;
}

// 関数定義
void _gen_function_difinition(Node *node) {
  _gen_prologue(node);
  gen(node->lhs());
  _gen_epilogue(node);
}

// ブロック
void _gen_block(Node *node) {
  for (int i = 0; i < node->nodes()->len(); ++i) {
    gen(node->nodes()->get(i));
    if (i < node->nodes()->len() - 1)
      std::cout << "  pop rax" << std::endl;
  }
}

// return文
void _gen_return(Node *node) {
  gen(node->lhs());
  std::cout << "  pop rax" << std::endl;
  std::cout << "  mov rsp, rbp" << std::endl;
  std::cout << "  pop rbp" << std::endl;
  std::cout << "  ret" << std::endl;
}

// if文(elseなし)
void _gen_if(Node *node) {
  int labelNo = newLabelNo();
  gen(node->lhs());
  std::cout << "  pop rax" << std::endl;
  std::cout << "  cmp rax, 0" << std::endl;
  std::cout << "  push rax" << std::endl; // NOTE: 動かないから入れてみた(ifに入らなかった時にスタックずれる気がする)
  std::cout << "  je  .Lend" << labelNo << std::endl;
  // NOTE: ここにもpop入れないとスタックずれてる気がする
  std::cout << "  pop rax" << std::endl;
  gen(node->rhs());
  std::cout << ".Lend" << labelNo << ":" << std::endl;
}

// if文(elseあり)
void _gen_if_else(Node *node) {
  int labelNo1 = newLabelNo();
  int labelNo2 = newLabelNo();
  gen(node->lhs());
  std::cout << "  pop rax" << std::endl;
  std::cout << "  cmp rax, 0" << std::endl;
  std::cout << "  je  .Lelse" << labelNo1 << std::endl;
  gen(node->rhs());
  std::cout << "  jmp .Lend" << labelNo2 << std::endl;
  std::cout << ".Lelse" << labelNo1 << ":" << std::endl;
  gen(node->node1());
  std::cout << ".Lend" << labelNo2 << ":" << std::endl;
}

// while文
void _gen_while(Node *node) {
  int labelNo1 = newLabelNo();
  int labelNo2 = newLabelNo();
  std::cout << ".Lbegin" << labelNo1 << ":" << std::endl;
  gen(node->lhs());
  std::cout << "  pop rax" << std::endl;
  std::cout << "  cmp rax, 0" << std::endl;
  std::cout << "  je  .Lend" << labelNo2 << std::endl;
  gen(node->rhs());
  std::cout << "  jmp .Lbegin" << labelNo1 << std::endl;
  std::cout << ".Lend" << labelNo2 << ":" << std::endl;
}

// for文
void _gen_for(Node *node) {
  int labelNo1 = newLabelNo();
  int labelNo2 = newLabelNo();
  gen(node->lhs());
  std::cout << ".Lbegin" << labelNo1 << ":" << std::endl;
  gen(node->rhs());
  std::cout << "  pop rax" << std::endl;
  std::cout << "  cmp rax, 0" << std::endl;
  std::cout << "  je  .Lend" << labelNo2 << std::endl;
  gen(node->node2());
  gen(node->node1());
  std::cout << "  jmp .Lbegin" << labelNo1 << std::endl;
  std::cout << ".Lend" << labelNo2 << ":" << std::endl;
}

// 関数呼び出しの引数の処理
void _gen_function_call_args(Node *node) {
  // 引数の処理(Linux ABI)
  for (int i = node->nodes()->len() - 1; i >= 0; --i) {
    // 引数を逆順で渡す
    // rdi, rsi, rdx, rcx, r8, r9, スタック…
    Node *param = node->nodes()->get(i);
    switch (i) {
    case 0:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov rdi, rax" << std::endl;
      break;
    case 1:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov rsi, rax" << std::endl;
      break;
    case 2:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov rdx, rax" << std::endl;
      break;
    case 3:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov rcx, rax" << std::endl;
      break;
    case 4:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov r8, rax" << std::endl;
      break;
    case 5:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov r9, rax" << std::endl;
      break;
    default:
      gen(param);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  push rax" << std::endl;
      break;
    }
  }
}

// 関数呼び出し
void _gen_function_call(Node *node) {
  // 引数
  _gen_function_call_args(node);
  // 呼び出し
  std::cout << "  call " << node->name() << std::endl;
  std::cout << "  push rax" << std::endl;
}

// ポインタ演算の前処理
void _gen_pointer_calculation(Node *node) {
  if (node->lhs()->ty() == ND_IDENT) {
    switch (node->lhs()->symbol()->type()->ty()) {
    case INT:
      break;
    case PTR:
      // ポインタが指す型の大きさを掛けてから加算
      // TODO: 固定で4になってしまっている
      // TODO: 右辺やポインタのポインタや演算子の入れ子も考える
      std::cout << "  push rax" << std::endl;
      std::cout << "  mov rax, rdi" << std::endl;
      std::cout << "  mov rsi, 4" << std::endl;
      std::cout << "  imul rsi" << std::endl;
      std::cout << "  mov rdi, rax" << std::endl;
      std::cout << "  pop rax" << std::endl;
      break;
    }
  }
}

// 二項演算子
void _gen_binary_operator(Node *node) {
  // 左辺の生成
  gen(node->lhs());
  // 右辺の生成
  gen(node->rhs());

  // 右辺
  std::cout << "  pop rdi" << std::endl;
  // 左辺
  std::cout << "  pop rax" << std::endl;

  // 二項演算子の生成
  switch (node->ty()) {
  case '+':
    _gen_pointer_calculation(node);
    std::cout << "  add rax, rdi" << std::endl;
    break;
  case '-':
    _gen_pointer_calculation(node);
    std::cout << "  sub rax, rdi" << std::endl;
    break;
  case '*':
    std::cout << "  imul rdi" << std::endl;
    break;
  case '/':
    std::cout << "  cqo" << std::endl;
    std::cout << "  idiv rdi" << std::endl;
    break;
  case ND_EQ: // '=='
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  sete al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_NE: // '!='
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setne al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case '<':
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setl al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_LE: // '<='
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setle al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case '>':
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setg al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_GE: // '>='
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setge al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  }

  std::cout << "  push rax" << std::endl;
}

// コード生成
void gen(Node *node) {
  // 文
  switch (node->ty()) {
  case ND_FDEFIN:
    _gen_function_difinition(node);
    return;
  case ND_BLOCK:
    _gen_block(node);
    return;
  case ND_RETURN:
    _gen_return(node);
    return;
  case ND_IF:
    _gen_if(node);
    return;
  case ND_IFELSE:
    _gen_if_else(node);
    return;
  case ND_WHILE:
    _gen_while(node);
    return;
  case ND_FOR:
    _gen_for(node);
    return;
  }

  // 単項演算子('+', '-'は式か単項演算子に変換されている)
  switch (node->ty()) {
  case '*':
    // 間接参照演算子
    if (node->rhs() != NULL) // 二項演算子しかチェック
      break;
    // そのまま変数の値がアドレスとして入っているはず
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rax, [rax]" << std::endl;
    std::cout << "  push rax" << std::endl;
    return;
  case '&':
    // アドレス演算子
    if (node->rhs() != NULL) // 二項演算子しかチェック
      break;
    // 変数のアドレスをpush
    _gen_lval(node->lhs());
    return;
  }

  // 数値
  if (node->ty() == ND_NUM) {
    std::cout << "  push " << node->val() << std::endl;
    return;
  }

  // 識別子
  // 変数のアドレスから値を取得してpush
  if (node->ty() == ND_IDENT) {
    _gen_lval(node);
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rax, [rax]" << std::endl;
    std::cout << "  push rax" << std::endl;
    return;
  }

  // 代入
  // 変数のアドレスへ右辺値を代入
  if (node->ty() == '=') {
    _gen_lval(node->lhs());
    gen(node->rhs());

    std::cout << "  pop rdi" << std::endl;
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov [rax], rdi" << std::endl;
    std::cout << "  push rdi" << std::endl;
    return;
  }

  // 関数呼び出し
  if (node->ty() == ND_FCALL) {
    _gen_function_call(node);
    return;
  }

  if (node->ty() == ND_VDEFIN) {
    // 何もしない
    std::cout << "  push rax" << std::endl;
    return;
  }

  // 二項演算子
  _gen_binary_operator(node);
}
