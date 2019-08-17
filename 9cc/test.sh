#!/bin/bash
try(){
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# whether 'simple number parsing' works
try 0 "0;"

# whether 'four arithmetic operations parsing' works
try 42 "42;"
try 21 "5+20-4;"
try 41 " 12 + 34 - 5;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 1 "+1;"
try 10 "-10 + 20;"

# whether 'comparison operator parsing' works
try 0 "1 < 1;"
try 1 "1 < 2;"
try 0 "1 > 1;"
try 1 "2 > 1;"
try 0 "1 <= 0;"
try 1 "1 <= 1;"
try 0 "0 >= 1;"
try 1 "1 >= 1;"
try 0 "1 == 0;"
try 1 "1 == 1;"
try 0 "1 != 1;"
try 1 "1 != 0;"
try 1 "1 + 4 > 0 + 1 == 2 > 1;"

# whether 'simple local variable parsing' works
try 0 "s = 0;"
try 0 "s = 0;"
try 3 "a = 3; a;"
try 22 "b = 5 * 6 - 8;"
try 1 "a = 4; a == 4;"
try 8 "a = 3; b = 5; a + b;"

# whether 'multiple strings local variable parsing' works
try 20 "value = 20;"
try 100 "ab = 50; dc=50; ab + dc;"

# whether 'complex local variable parsing' works
try 20 "test_val = 20;"
try 40 "test1 = 20; test2=20; test1 + test2;"

# whether 'return parsing' works
try 1 "return 1;"
try 5 "i = 5; return i;"
try 10 "i = 10; return i; 1;"

# whether 'if parsing' works
try 1 "if(1) return 1;"
try 2 "if(0) return 1; 2;"
try 1 "if(1 < 0) return 3; return 1;"

# wheather 'else parsing' works
try 2 "if(0) 1; else 2;"
try 1 "if(1) 1; else 2;"

# whether 'while parsing' works
try 1 "while(1 < 0) return 3; return 1;"
try 5 "i = 0; while(5 > i) i = i + 1; return i;"

# whether 'for parsing' works
try 5 "test = 0; for(i=0; 5>i; i=i+1) test=test+1; return test;"
try 5 "test = 0; for(;5>test;) test=test+1; return test;"
try 5 "test = 0; for(i=0; ; i=i+1) if(i > 4) return i;"

# whether 'unless parsing' works
try 1 "unless(0) return 1;"
try 2 "unless(1) return 1; 2;"
try 1 "unless(1 > 0) return 3; return 1;"

# wheather 'else(unless) parsing' works
try 2 "unless(1) 1; else 2;"
try 1 "unless(0) 1; else 2;"

echo OK