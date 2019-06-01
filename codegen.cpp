#include <iostream>

#include "codegen.h"

// コード生成
void gen(Node *node) {
  if (node->ty() == ND_NUM) {
    std::cout << "  push " << node->val() << std::endl;
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
