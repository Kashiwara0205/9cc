#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined vector
typedef struct Vector Vector;

struct Vector{
  void **data;  // array's data
  int capacity; // array's capacity
  int len;      // array's length
};

Vector *new_vec();
void vec_push(Vector *vec, void *data_address);

// for user input 
extern char *user_input;

// defind token
typedef enum{
  TK_RESERVED,   // symbol
  TK_IDENT,      // identifier
  TK_NUM,        // number
  TK_EOF,        // end of token
  TK_RETURN,     // return
  TK_IF,         // if
  TK_ELSE,       // else
  TK_WHILE,      // while
  TK_FOR,        // for
  TK_UNLESS,     // unless
  TK_BLOCK,      // { }
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
Token *consume_ident();

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
  ND_EQ,          // ==
  ND_NOT_EQ,      // !=
  ND_LT,          // <
  ND_LT_EQ,       // <=
  ND_RT,          // >
  ND_RT_EQ,       // >=
  ND_ADD,         // +
  ND_SUB,         // -
  ND_MUL,         // *
  ND_DIV,         // /
  ND_ASSIGN,      // =
  ND_LVAR,        // local variable
  ND_NUM,         // number
  ND_RETURN,      // return
  ND_IF,          // if
  ND_ELSE,        // else
  ND_WHILE,       // while
  ND_FOR,         // for
  ND_UNLESS,      // unless
  ND_UNLESS_ELSE, // else(unless)
  ND_BLOCK        // { }
} NodeKind;

typedef struct Node Node;

// node type
struct Node{
  NodeKind kind;      // node kind
  Node *lhs;          // left side
  Node *rhs;          // right side
  int val;            // if kind is ND_NUM, only use
  int offset;         // if kind is ND_LVAR, only use
  Node *conditional;  // if kind is ND_IF, ND_WHILE, ND_FOR, ND_UNLESS only use
  Node *content;      // if kind is ND_IF, ND_WHILE, ND_FOR, ND_UNLESS only use
  Node *else_content; // if kind is ND_ELSE, ND_UNLESS_ELSE, only use
  Node *init;         // if kind is ND_FOR
  Node *iter_expr;    // if kind is ND_FOR
  Vector *stmts;      // if kind is ND_BLOK
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
