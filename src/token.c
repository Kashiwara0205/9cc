#include "9cc.h"

// current token
Token *token;
// current variable
LVar *locals;

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
    *p == '=' || *p == ',';

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

bool is_variable(char *p){
  bool result = 
    ('a' <= *p && *p <= 'z') || 
    ('0' <= *p && *p <= '9') || 
    ('_' == *p);

  return result;
}

bool is_alnum(char c) {
  bool result = 
    ('a' <= c && c <= 'z') ||
    ('A' <= c && c <= 'Z') ||
    ('0' <= c && c <= '9') ||
    (c == '_');

  return result;
}

bool is_return(char *p){
  // '!is_alnum(p[6])' block case, for example returnx, return_1
  return strncmp(p, "return", 6) == 0 && !is_alnum(p[6]);
}

bool is_if(char *p){
  // '!is_alnum(p[2])' block case, for example ifx, if_1
  return strncmp(p, "if", 2) == 0 && !is_alnum(p[2]);
}

bool is_else(char *p){
  // '!is_alnum(p[2])' block case, for example elsex, else_1
  return strncmp(p, "else", 4) == 0 && !is_alnum(p[4]);
}

bool is_while(char *p){
  // '!is_alnum(p[5])' block case, for example whilex, while_1
  return strncmp(p, "while", 5) == 0 && !is_alnum(p[5]);
}

bool is_for(char *p){
  // '!is_alnum(p[3])' block case, for example forx, for_1
  return strncmp(p, "for", 3) == 0 && !is_alnum(p[3]);
}

bool is_unless(char *p){
  // '!is_alnum(p[6])' block case, for example unlessx, unless_1
  return strncmp(p, "unless", 6) == 0 && !is_alnum(p[6]);
}

bool is_block(char *p){
  return  strncmp(p, "{", 1) == 0 || strncmp(p, "}", 1) == 0;
}

int get_variable_offset(char *p){
  int variable_length = 0;
  char *current_position = p;
  while(is_variable(p)){
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

    if(is_return(user_input)) {
      cur = new_token(TK_RETURN, cur, user_input, 6);
      user_input+=6;
      continue;
    }

    if(is_if(user_input)){
      cur = new_token(TK_IF, cur, user_input, 2);
      user_input+=2;
      continue;
    }

    if(is_else(user_input)){
      cur = new_token(TK_ELSE, cur, user_input, 4);
      user_input+=4;
      continue;
    }

    if(is_while(user_input)){
      cur = new_token(TK_WHILE, cur, user_input, 5);
      user_input+=5;
      continue;
    }

    if(is_for(user_input)){
      cur = new_token(TK_FOR, cur, user_input, 3);
      user_input+=3;
      continue;
    }

    if(is_unless(user_input)){
      cur = new_token(TK_UNLESS, cur, user_input, 6);
      user_input+=6;
      continue;
    }

    if(is_block(user_input)){
      cur = new_token(TK_BLOCK, cur, user_input, 1);
      user_input+=1;
      continue;
    }

    if(is_lowercase_alpha(user_input)) {
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