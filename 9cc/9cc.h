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
  TK_IDENT,      // identifier
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
Token *tokenize();

// for local variable type
typedef struct LVar LVar;

struct LVar{
  LVar *next;  // next valirable is nil
  char *name;  // variable's name
  int len;     // variable's length
  int offset;  // offset from RBP(register base pointer)
};

// local variable
extern LVar *locals;

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
  ND_ASSIGN, // =
  ND_LVAR,   // local variable
  ND_NUM,    // number
} NodeKind;

typedef struct Node Node;

// node type
struct Node{
  NodeKind kind;  // node kind
  Node *lhs;      // left side
  Node *rhs;      // right side
  int val;        // if kind is ND_NUM, only use
  int offset;     // if kind is ND_LVAR, only use
};

// proto type decration for Node
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();
Node *unary();
Node *assign();
Node *stmt();

void gen(Node *node);

// for error
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// Use one per semicolon
// for example a; 1+1; → [0]: a [1]: 1+1
Node *code[100];

void program();