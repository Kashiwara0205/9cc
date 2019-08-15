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