#include "9cc.h"

Vector *new_vec(){
  Vector *vec = malloc(sizeof(Vector));
  int first_size = 16;
  vec->data = malloc(sizeof(void *) * first_size);
  vec->capacity = first_size;
  vec->len = 0;
  return vec;
}

void vec_push(Vector *vec, void *elem){
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}