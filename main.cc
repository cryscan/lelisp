#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "common.h"

using namespace std;
atom atom_true, atom_quote, atom_lambda;

int main(int argc, char** argv)
{
    env* e = cons(0);
    register_func(e);
    
    atom_true = create_atom("t");
    atom_lambda = create_atom("lambda");
    atom_quote = create_atom("quote");

    while (true)
	execute(e, cin);
  
    return 0;
}
