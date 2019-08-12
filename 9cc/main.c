#include "9cc.h"

// for user input 
char *user_input = NULL;

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "mismatch numbers of argument\n");
    return 1;
  }

  // tokenize
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // out put assembler
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // syntax tree
  gen(node);

  // pop calculation result, and return
  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}