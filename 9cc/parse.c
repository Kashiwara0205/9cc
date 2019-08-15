#include "9cc.h"

// current token
Token *token;
// current variable
LVar *locals;

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

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return false;
  Token *ident_token;

  // copy ident_token address 
  ident_token = token;
  token = token->next;

  return ident_token;
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
    *p == '<' || *p == '>' || *p == ';' ||
    *p == '=';
  return result;
}

bool isdouble_symbol(char *p){
  bool result =
    strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
    strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0;

  return result;
}

bool is_lowercase_alpha(char *p){
  bool result = 'a' <= *p && *p <= 'z';

  return result;
}

int get_variable_offset(char *p){
  int variable_length = 0;
  char *current_position = p;
  while(is_lowercase_alpha(p)){
    variable_length += 1;
    p += 1;
  }
  p = current_position;

  return variable_length;
}

// return tokenized p which is inputed 
Token *tokenize() {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  while (*user_input){
    // skip space
    if (isspace(*user_input)){
      user_input+=1;
      continue;
    }

    if(isdouble_symbol(user_input)){
      cur = new_token(TK_RESERVED, cur, user_input, 2);
      // skip double_symbol ex).' == '
      user_input+=2;
      continue;
    }

    if(issymbol(user_input)){
      cur = new_token(TK_RESERVED, cur, user_input, 1);
      // skip symbol ex).' > '
      user_input+=1;
      continue;
    }

    if(isdigit(*user_input)){
      cur = new_token(TK_NUM, cur, user_input, 1);
      cur->val = strtol(user_input, &user_input, 10);
      continue;
    }

    if (is_lowercase_alpha(user_input)) {
      // memo:
      // token structure, when register variable token
      // | hoge | = 1 
      // variable_length: 4
      // after add offset to user_input 
      //  = 1
      int offset = get_variable_offset(user_input);
      cur = new_token(TK_IDENT, cur, user_input, offset);

      user_input += offset;
      continue;
    }

    error("doesn't tokenize");
  }

  new_token(TK_EOF, cur, user_input, 0);
  token = head.next;
}

LVar *find_lvar(Token *tok){
  for (LVar *var = locals; var; var = var->next)
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
    return NULL;
}

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

Node *stmt() {
  Node *node = expr();
  expect(';');
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
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
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      // save local variable
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals == NULL){
        lvar->offset = 8;
      }else{
        lvar->offset = locals->offset + 8;
      }
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}