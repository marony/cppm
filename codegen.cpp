#include <iostream>

#include "codegen.h"
#include "cppm.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

// 左辺値
// 変数のアドレスをpush
void gen_lval(Node *node) {
  if (node->ty() != ND_IDENT)
    error("代入の左辺値が変数ではありません");

  int offset = node->offset();
  std::cout << "  mov rax, rbp" << std::endl;
  std::cout << "  sub rax, " << offset << std::endl;
  std::cout << "  push rax" << std::endl;
}

int labelNo = 0;

int newLabelNo() {
  return ++labelNo;
}

// コード生成
void gen(Node *node) {
  // return
  if (node->ty() == ND_RETURN) {
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rsp, rbp" << std::endl;
    std::cout << "  pop rbp" << std::endl;
    std::cout << "  ret" << std::endl;
    return;
  } else if (node->ty() == ND_IF) {
    int labelNo = newLabelNo();
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  cmp rax, 0" << std::endl;
    std::cout << "  push rax" << std::endl; // 動かないから入れてみた
    std::cout << "  je  .Lend" << labelNo << std::endl;
    // ここにpop入れないとスタックずれてるか？
    gen(node->rhs());
    std::cout << ".Lend" << labelNo << ":" << std::endl;
    return;
  } else if (node->ty() == ND_IFELSE) {
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
    return;
  } else if (node->ty() == ND_WHILE) {
    return;
  } else if (node->ty() == ND_FOR) {
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
    gen_lval(node);
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rax, [rax]" << std::endl;
    std::cout << "  push rax" << std::endl;
    return;
  }

  // 代入
  // 変数のアドレスへ右辺値を代入
  if (node->ty() == '=') {
    gen_lval(node->lhs());
    gen(node->rhs());

    std::cout << "  pop rdi" << std::endl;
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov [rax], rdi" << std::endl;
    std::cout << "  push rdi" << std::endl;
    return;
  }

  // 左辺の生成
  gen(node->lhs());
  // 右辺の生成
  gen(node->rhs());

  // 右辺
  std::cout << "  pop rdi" << std::endl;
  // 左辺
  std::cout << "  pop rax" << std::endl;

  // 演算の生成
  switch (node->ty()) {
  case '+':
    std::cout << "  add rax, rdi" << std::endl;
    break;
  case '-':
    std::cout << "  sub rax, rdi" << std::endl;
    break;
  case '*':
    std::cout << "  imul rdi" << std::endl;
    break;
  case '/':
    std::cout << "  cqo" << std::endl;
    std::cout << "  idiv rdi" << std::endl;
    break;
  case ND_EQ:
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  sete al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_NE:
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setne al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case '<':
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setl al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_LE:
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setle al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case '>':
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setg al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  case ND_GE:
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setge al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    break;
  }

  std::cout << "  push rax" << std::endl;
}
