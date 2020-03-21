#include "9cc.h"

// for user input 
char *user_input = NULL;

int main(int argc, char **argv){

  if (argc != 2){
    fprintf(stderr, "mismatch numbers of argument\n");
    return 1;
  }

  user_input = argv[1];
  // run tokenize in token.c for tokenize
  tokenize();
  // run program in node.c for parsing
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  
  for (int i = 0; functions[i]; i++) {
    gen_func(functions[i]);
  }

  return 0;
}