#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "common.h"

using namespace std;
atom atom_true, atom_quote, atom_lambda;

bool paren_matched(const string &str)
{
    int count = 0;
    for (auto ch : str)
    {
	if (ch == '(')
	    ++count;
	else if (ch == ')')
	    --count;
    }
    assert(count >= 0);
    return count == 0;
}

void print(void* v)
{
    if (v == 0)
	cout << "()";
    else if (is_atom(v))
	cout << *(atom)v;
    else
    {
	cout << "(";
	list* l = (list*)v;
	while (l)
	{
	    print(l->head);
	    l = l->tail;
	    if(l)
		cout << " ";
	}
	cout << ")";
    }
}

void run_command(env* e)
{
    string line, next;
    while (true)
    {
	cout << "> ";
	getline(cin, line);
	while (!paren_matched(line))
	{
	    cout << "  ";
	    getline(cin, next);
	    line = line + "\n" + next;
	}
	parcel r = parse(line.cbegin());
	if (r.content == 0)
	    continue;
	print(eval(e, (list*)r.content));
	cout << endl;
	clean(e);
    }
}

int main(int argc, char** argv)
{
    env* e = new env;
    atom_true = create_atom("t");
    atom_lambda = create_atom("lambda");
    atom_quote = create_atom("quote");
    
    register_func(e);
    run_command(e);
  
    return 0;
}
