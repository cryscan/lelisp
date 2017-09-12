BIN=lelisp
SRC=main.cc common.cc parser.cc evaluator.cc execute.cc
INCLUDE=common.h
LIB=
CC=g++
CC_FLAG=-std=c++14 -g

all:$(BIN)

$(BIN):$(SRC) $(INCLUDE)
	$(CC) $(LIB) $(CC_FLAG) -o $@ $^

clean:
	-rm -f *.o $(BIN)
