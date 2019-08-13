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

  tokenize();
  program();

  // out put assembler
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  // get 26 variable's area
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i = 0; code[i]; i++) {
    // use syntax tree to generate code
    gen(code[i]);
    printf("  pop rax\n");
  }

  // epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");


  return 0;
}