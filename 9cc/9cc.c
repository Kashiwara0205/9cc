#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defind token
typedef enum{
  TK_RESERVED,   // symbol
  TK_NUM,        // number
  TK_EOF,        // end of token
} TokenKind;

typedef struct Token Token;

// token type
struct Token{
  TokenKind kind; // token kind
  Token *next;    // next token
  int val;        // if kind is TK_NUM, input number
  char *str;      // token str
  int len;
};

// current token
Token *token;

// for out put error function
void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

char *user_input;

void error_at(char *loc, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  // move to error area
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// if next token is symbol, read next token and return true
// otherwise return false
bool consume(char op){
  if (token->kind != TK_RESERVED || token ->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// if next token is symbol, read next token
// otherwise notice error
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error("Not '%c'", op);

  token = token->next;
}

// if next token is number, read next token and return value
// totherwise notice error
int expect_number(){
  if (token->kind != TK_NUM)
    error_at(token->str, "Not Number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

// make new token, and add new token to cur
Token *new_token(TokenKind kind, Token *cur, char *str){
  Token *tok = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> str = str;
  cur -> next = tok;
  return tok;
}

// return tokenized p which is inputed 
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p){
    // skip space
    if (isspace(*p)){
      p++;
      continue;
    }

    if(*p == '+' || *p == '-' || *p == '*' || 
       *p == '/' || *p == '(' || *p == ')'){
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("doesn't tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

// defined syntax tree's node
typedef enum{
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_NUM,  // number
} NodeKind;

typedef struct Node Node;

// node type
struct Node{
  NodeKind kind;  // node kind
  Node *lhs;      // left side
  Node *rhs;      // right side
  int val;        // if kind is ND_NUM, input number
};

// proto type
Node *expr();
Node *mul();
Node *term();
Node *unary();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node -> kind = ND_NUM;
  node -> val = val;
  return node;
}

Node *expr() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*'))
      node = new_node(ND_MUL, node, unary());
    else if (consume('/'))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if (consume('+'))
    return term();
  if (consume('-'))
    return new_node(ND_SUB, new_node_num(0), term());
  return term();
}

Node *term() {
  if (consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}

void gen(Node *node){
  if (node->kind == ND_NUM){
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind){
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }

  printf("  push rax\n");
}

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