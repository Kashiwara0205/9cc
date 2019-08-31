#include "9cc.h"
// if next token is symbol, read next token and return true
// otherwise return false
bool consume(char *op, int token_kind) {
  if (token->kind != token_kind ||
      strlen(op) != token->len || 
      memcmp(token->str, op, token->len)){
    return false;
  }
  token = token->next;

  return true;
}

// if next token is variable, read next token and return token
// otherwise return false
Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return false;
  Token *ident_token;
  // copy ident_token address 
  ident_token = token;
  token = token->next;

  return ident_token;
}

// if next token is ident, read next token and return value's name
// totherwise notice error
char *expect_ident(){
  if (token->kind != TK_IDENT)
    error_at(token->str, "Not Ident");
  Token *ident_token;
  // copy ident_token address 
  ident_token = token;
  token = token->next;

  return strndup(ident_token->str, ident_token->len);
}

// if next token is symbol, read next token
// otherwise notice error
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error("Not '%c'", op);
  token = token->next;
}

void expect_block(char op){
  if(token->kind != TK_BLOCK || token->str[0] != op)
    error("Not BLOCK");
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

// wheter end of file
bool at_eof(){
  return token->kind == TK_EOF;
}

LVar *find_lvar(Token *tok){
  for (LVar *var = locals; var; var = var->next)
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
    return NULL;
}

LVar *gen_lvar(Token *ident_token, Node *node){
  LVar *lvar = find_lvar(ident_token);
  if (lvar) {
    node->offset = lvar->offset;
  } else {
    // save local variable
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = ident_token->str;
    lvar->len = ident_token->len;
    if (locals == NULL){
      lvar->offset = 8;
    }else{
      lvar->offset = locals->offset + 8;
    }
    node->offset = lvar->offset;
    locals = lvar;
  }
}

Vector *get_arguments(){
  if(consume(")", TK_RESERVED)){
    return new_vec();
  }else{
    Vector *args = new_vec();
    do {
      vec_push(args, (void *)term());
    }while(consume(",", TK_RESERVED));
    expect(')');
    return args;
  }
}

Vector *get_function_arguments_name(){
  if(consume(")", TK_RESERVED)){
    return new_vec();
  }else{
    Vector *args = new_vec();
    do {
      Token *tok = consume_ident();
      Node *node = calloc(1, sizeof(Node));
      node->kind = ND_LVAR;
      gen_lvar(tok, node);
      vec_push(args, (void *)node);
    }while(consume(",", TK_RESERVED));
    expect(')');
    return args;
  }
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
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Function *parse_function(){
  Function *function = calloc(1, sizeof(Function));
  function->name = expect_ident();
  expect('(');
  function->arguments_name = get_function_arguments_name();
  expect_block('{');
  Vector *vec = new_vec();
  while(!consume("}", TK_BLOCK)){
    vec_push(vec, (void *)stmt());
  }
  function -> stmts = vec;
  return function;
}

Node *stmt() {
  Node *node = calloc(1, sizeof(Node));

  if(consume("if", TK_IF)){
    expect('(');
    node->conditional = expr();
    expect(')');
    node->content = stmt();

    if(consume("else", TK_ELSE)){
      node->kind = ND_ELSE;
      node->else_content = stmt();
    }else{
      node->kind = ND_IF;
    }

    return node;
  }
  
  if(consume("unless", TK_UNLESS)){
    expect('(');
    node->conditional = expr();
    expect(')');
    node->content = stmt();

    if(consume("else", TK_ELSE)){
      node->kind = ND_UNLESS_ELSE;
      node->else_content = stmt();
    }else{
      node->kind = ND_UNLESS;
    }

    return node;
  }
  
  if(consume("while", TK_WHILE)){
    node->kind = ND_WHILE;
    expect('(');
    node->conditional = expr();
    expect(')');
    node->content = stmt();

    return node;
  }

  if(consume("for", TK_FOR)){
    node->kind = ND_FOR;
    expect('(');

    // parse 'init' phase
    // for(init; conditional; iter_expr)
    if(!consume(";", TK_RESERVED)){
      node->init = expr();
      expect(';');
    }else{
      node->init = false;
    }

    // parse 'conditional' phase
    // for(init; conditional; iter_expr)
    if(!consume(";", TK_RESERVED)){
      node->conditional = expr();
      expect(';');
    }else{
      node->conditional = false;
    }

    // parse 'iter_expr' phase
    // for(init; conditional; iter_expr)
    if(!consume(")", TK_RESERVED)){
      node->iter_expr = expr();
      expect(')');
    }else{
      node->iter_expr = false;
    }

    node->content = stmt();

    return node;
  }

  if(consume("{", TK_BLOCK)){
    Vector *vec = new_vec();
    while (!consume("}", TK_BLOCK)) {
      // use (void *) for convert data to address
      vec_push(vec, (void *)stmt());
    }

    node->kind = ND_BLOCK;
    node->stmts = vec;

    return node;
  }

  if(consume("return", TK_RETURN)){
    node = new_node(ND_RETURN, NULL, expr());
  }else{
    node = expr();
  }

  expect(';');
  return node;
}

Node *expr() {
  Node *node;
  node = assign();

  return node;
}

Node *assign() {
  Node *node = equality();
  if (consume("=", TK_RESERVED))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("==", TK_RESERVED))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!=", TK_RESERVED))
      node = new_node(ND_NOT_EQ, node, relational());
    else
      return node;
  }

  return node;
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<", TK_RESERVED))
      node = new_node(ND_LT, node, add());
    else if (consume("<=", TK_RESERVED))
      node = new_node(ND_LT_EQ, node, add());
    else if (consume(">", TK_RESERVED))
      node = new_node(ND_RT, node, add());
    else if (consume(">=", TK_RESERVED))
      node = new_node(ND_RT_EQ, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+", TK_RESERVED))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-", TK_RESERVED))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*", TK_RESERVED))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/", TK_RESERVED))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if (consume("+", TK_RESERVED))
    return term();
  if (consume("-", TK_RESERVED))
    return new_node(ND_SUB, new_node_num(0), term());
  return term();
}

Node *term() {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));

    if(!consume("(", TK_RESERVED)){
      // token is loacal variable
      node->kind = ND_LVAR;
      gen_lvar(tok, node);
    }else{
      // token is function
      Vector *args = get_arguments();
      node->kind = ND_FUNC_CALL;
      node->function_name = strndup(tok->str, tok->len);
      node->arguments = args;
    }
    
    return node;
  }

  if (consume("(", TK_RESERVED)) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}

void program() {
  int i = 0;
  while (!at_eof()){
    functions[i++] = parse_function();
  }
  functions[i] = NULL;
}