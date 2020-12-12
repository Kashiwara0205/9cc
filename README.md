# 9cc
[![Build Status](https://travis-ci.org/Kashiwara0205/9cc.svg?branch=master)](https://travis-ci.org/Kashiwara0205/9cc)


## Usage

compile example by using shell script
```
try(){
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s test_functions.o
  ./tmp
  actual="$?"
  
  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}
```

## Test

```
$ cd src
$ make test
```

## Reference page
[低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)  
[dekokunさんのgithub](https://github.com/dekokun/9cc)
