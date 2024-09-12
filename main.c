#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

char *src;
int pos;

float additive();

void skip_ws() {
  while (isspace(src[pos]))
    pos++;
}

float num() {
  int a = 0;
  while (isdigit(src[pos])) {
    a = (a * 10) + (src[pos] - '0');
    pos++;
  }
  if (src[pos] != '.') {
    return a * 1.0;
  }
  pos++;
  float b = 0.0;
  float div = 10.0;
  while (isdigit(src[pos])) {
    b += (src[pos] - '0') * 1.0 / div;
    div *= 10;
    pos++;
  }
  return a + b;
}

float primary() {
  skip_ws();
  char c = src[pos];
  if (c == '-') {
    pos++;
    return -primary();
  }
  if (isdigit(src[pos])) {
    return num();
  }
  if (c == '(') {
    pos++;
    float inside = additive();
    if (src[pos] != ')') {
      fprintf(stderr, "error: expected closing parenthesis at pos:%d\n", pos);
      exit(1);
    }
    pos++;
    return inside;
  }
  fprintf(stderr, "error: unexpected '%c' at pos:%d\n", src[pos], pos);
  exit(1);
}

float power() {
  float lhs = primary();
  if (src[pos] != '^') {
    return lhs;
  }
  pos++;
  float rhs = primary();
  return powf(lhs, rhs);
}

float multiplicative() {
  float lhs = power();
  skip_ws();
  if (src[pos] != '*' && src[pos] != '/') {
    return lhs;
  }
  int is_div = src[pos] == '/';
  pos++;
  float rhs = multiplicative();
  if (is_div) {
    if (rhs < 0.0000001) {
      fprintf(stderr, "error: division by 0 at pos:%d\n", pos);
      exit(1);
    }
    return lhs / rhs;
  } else {
    return lhs * rhs;
  }
}

float additive() {
  float lhs = multiplicative();
  skip_ws();
  if (src[pos] != '+' && src[pos] != '-') {
    return lhs;
  }
  float sign = src[pos] == '+' ? 1.0 : -1.0;
  pos++;
  float rhs = additive();
  return lhs + sign * rhs;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "error: expected exactly one argument\n");
    fprintf(stderr, "usage: %s '2 * (3 + 3)^4'\n", argv[0]);
    return 1;
  }
  src = argv[1];
  pos = 0;
  float result = additive();
  printf("result: %f \n", result);
  return 0;
}
