#include "9cc.h"

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
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len || 
      memcmp(token->str, op, token->len)){
    return false;
  }
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  Token *tok = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> str = str;
  // set token length, if it is '==', then 2
  tok -> len = len;
  cur -> next = tok;
  return tok;
}

// for tokenize
bool issymbol(char *p){
  bool result = 
    *p == '+' || *p == '-' || *p == '*' || 
    *p == '/' || *p == '(' || *p == ')' ||
    *p == '<' || *p == '>';
  return result;
}

bool isdouble_symbol(char *p){
  bool result =
    strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
    strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0;

  return result;
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

    if(isdouble_symbol(p)){
      cur = new_token(TK_RESERVED, cur, p, 2);
      // skip double_symbol ex).' == '
      p+=2;
      continue;
    }

    if(issymbol(p)){
      cur = new_token(TK_RESERVED, cur, p, 1);
      // skip symbol ex).' > '
      p+=1;
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("doesn't tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

// proto type declaration

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
  Node *node = equality();

  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NOT_EQ, node, relational());
    else
      return node;
  }

  return node;
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LT_EQ, node, add());
    else if (consume(">"))
      node = new_node(ND_RT, node, add());
    else if (consume(">="))
      node = new_node(ND_RT_EQ, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if (consume("+"))
    return term();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), term());
  return term();
}

Node *term() {
  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}