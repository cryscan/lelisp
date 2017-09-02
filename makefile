BIN=lelisp
SRC=main.cc common.cc parser.cc eval.cc
LIB=
CC=g++
CC_FLAG=-std=c++11 -g

all:$(BIN)

$(BIN):$(SRC)
	$(CC) $(LIB) $(CC_FLAG) -o $@ $^

clean:
	-rm -f *.o $(BIN)
