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
  if (node->ty() == ND_BLOCK) {
    debug("block");
    for (int i = 0; i < node->nodes().len(); ++i) {
      debug("%d\n", ((Node*)node->nodes().get(i))->ty());
      gen((Node*)node->nodes().get(i));
      if (i < node->nodes().len() - 1)
        std::cout << "  pop rax" << std::endl;
    }
    debug("kcolb");
    return;
  } else if (node->ty() == ND_RETURN) {
    debug("return (expr)");
    gen(node->lhs());
    debug("return");
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rsp, rbp" << std::endl;
    std::cout << "  pop rbp" << std::endl;
    std::cout << "  ret" << std::endl;
    debug("nruter");
    return;
  } else if (node->ty() == ND_IF) {
    debug("if (expr)");
    int labelNo = newLabelNo();
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  cmp rax, 0" << std::endl;
    std::cout << "  push rax" << std::endl; // 動かないから入れてみた(ifに入らなかった時にスタックずれる)
    std::cout << "  je  .Lend" << labelNo << std::endl;
    // ここにpop入れないとスタックずれてるか？
    debug("if");
    gen(node->rhs());
    std::cout << ".Lend" << labelNo << ":" << std::endl;
    debug("fi");
    return;
  } else if (node->ty() == ND_IFELSE) {
    debug("if (expr) else");
    int labelNo1 = newLabelNo();
    int labelNo2 = newLabelNo();
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  cmp rax, 0" << std::endl;
    std::cout << "  je  .Lelse" << labelNo1 << std::endl;
    debug("if else");
    gen(node->rhs());
    std::cout << "  jmp .Lend" << labelNo2 << std::endl;
    std::cout << ".Lelse" << labelNo1 << ":" << std::endl;
    debug("else");
    gen(node->node1());
    std::cout << ".Lend" << labelNo2 << ":" << std::endl;
    debug("esle fi");
    return;
  } else if (node->ty() == ND_WHILE) {
    debug("while (expr)");
    int labelNo1 = newLabelNo();
    int labelNo2 = newLabelNo();
    std::cout << ".Lbegin" << labelNo1 << ":" << std::endl;
    gen(node->lhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  cmp rax, 0" << std::endl;
    std::cout << "  je  .Lend" << labelNo2 << std::endl;
    debug("while");
    gen(node->rhs());
    std::cout << "  jmp .Lbegin" << labelNo1 << std::endl;
    std::cout << ".Lend" << labelNo2 << ":" << std::endl;
    debug("elihw");
    return;
  } else if (node->ty() == ND_FOR) {
    debug("for (expr1)");
    int labelNo1 = newLabelNo();
    int labelNo2 = newLabelNo();
    gen(node->lhs());
    std::cout << ".Lbegin" << labelNo1 << ":" << std::endl;
    debug("for (expr2)");
    gen(node->rhs());
    std::cout << "  pop rax" << std::endl;
    std::cout << "  cmp rax, 0" << std::endl;
    std::cout << "  je  .Lend" << labelNo2 << std::endl;
    debug("for (block)");
    gen(node->node2());
    debug("for (expr3)");
    gen(node->node1());
    std::cout << "  jmp .Lbegin" << labelNo1 << std::endl;
    std::cout << ".Lend" << labelNo2 << ":" << std::endl;
    debug("rof");
    return;
  }

  // 数値
  if (node->ty() == ND_NUM) {
    debug("num");
    std::cout << "  push " << node->val() << std::endl;
    debug("mun");
    return;
  }

  // 識別子
  // 変数のアドレスから値を取得してpush
  if (node->ty() == ND_IDENT) {
    debug("ident");
    gen_lval(node);
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov rax, [rax]" << std::endl;
    std::cout << "  push rax" << std::endl;
    debug("tnedi");
    return;
  }

  // 関数
  if (node->ty() == ND_FUNC) {
    debug("function");
    std::cout << "  call " << node->name() << std::endl;
    std::cout << "  push rax" << std::endl;
    // TODO: 引数の処理
    debug("noitcnuf");
    return;
  }

  // 代入
  // 変数のアドレスへ右辺値を代入
  if (node->ty() == '=') {
    debug("assign (lhs)");
    gen_lval(node->lhs());
    debug("assign (rhs)");
    gen(node->rhs());

    std::cout << "  pop rdi" << std::endl;
    std::cout << "  pop rax" << std::endl;
    std::cout << "  mov [rax], rdi" << std::endl;
    std::cout << "  push rdi" << std::endl;
    debug("ngissa");
    return;
  }

  // 左辺の生成
  debug("logop (lhs)");
  gen(node->lhs());
  // 右辺の生成
  debug("logop (rhs)");
  gen(node->rhs());
  debug("pogol");

  // 右辺
  std::cout << "  pop rdi" << std::endl;
  // 左辺
  std::cout << "  pop rax" << std::endl;

  // 演算の生成
  switch (node->ty()) {
  case '+':
    debug("plus");
    std::cout << "  add rax, rdi" << std::endl;
    debug("sulp");
    break;
  case '-':
    debug("minus");
    std::cout << "  sub rax, rdi" << std::endl;
    debug("sunim");
    break;
  case '*':
    debug("multiply");
    std::cout << "  imul rdi" << std::endl;
    debug("ylpitlum");
    break;
  case '/':
    debug("divide");
    std::cout << "  cqo" << std::endl;
    std::cout << "  idiv rdi" << std::endl;
    debug("edivid");
    break;
  case ND_EQ:
    debug("equal");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  sete al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("lauqe");
    break;
  case ND_NE:
    debug("not equal");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setne al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("lauqe ton");
    break;
  case '<':
    debug("less");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setl al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("ssel");
    break;
  case ND_LE:
    debug("less equal");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setle al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("lauqe ssel");
    break;
  case '>':
    debug("greater");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setg al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("retaerg");
    break;
  case ND_GE:
    debug("greater equal");
    std::cout << "  cmp rax, rdi" << std::endl;
    std::cout << "  setge al" << std::endl;
    std::cout << "  movzb rax, al" << std::endl;
    debug("lauqe retaerg");
    break;
  }

  std::cout << "  push rax" << std::endl;
}
