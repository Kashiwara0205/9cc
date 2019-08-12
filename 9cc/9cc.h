#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for user input 
extern char *user_input;

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
  int len;        // token length
};

// current token
extern Token *token;

// proto type decration for Token
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

// defined syntax tree's node
typedef enum{
  ND_EQ,     // ==
  ND_NOT_EQ, // !=
  ND_LT,     // <
  ND_LT_EQ,  // <=
  ND_RT,     // >
  ND_RT_EQ,  // >=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_NUM,    // number
} NodeKind;

typedef struct Node Node;

// node type
struct Node{
  NodeKind kind;  // node kind
  Node *lhs;      // left side
  Node *rhs;      // right side
  int val;        // if kind is ND_NUM, input number
};

// proto type decration for Node
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();
Node *unary();
void gen(Node *node);