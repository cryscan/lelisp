#include <iostream>
#include <string>
#include <stack>
#include "common.h"

using namespace std;

typedef void (*callback)(void);
stack<int> paren_stack;

bool paren_matched(const string& str)
{
    for (int i = 0; i < str.length(); ++i)
    {
	const char ch = str[i];
	if (ch == '(')
	    paren_stack.push(i);
	if (ch == ')')
	    paren_stack.pop();
    }
    return paren_stack.empty();
}

void print(void* expr)
{
    if (is_atom(expr))
	cout << *(atom)expr;
    else
    {
	cout << "(";
	list* l = (list*)expr;
	while (l)
	{
	    print(l->head);
	    l = l->tail;
	    if (l)
		cout << " ";
	}
	cout << ")";
    }
}

void execute(env* e, istream& s)
{
    bool done = false;
    string line, statement;

    cout << "> ";
    while (getline(s, line))
    {
	statement += "\n" + line;
	if (paren_matched(line))
	{
	    parcel r = parse(statement.cbegin());
	    print(eval(e, (list*)r.content));
	    cout << endl;
	    clean(e);
	    return;
	}
	else
	    for (int i = 0; i <= 2 + paren_stack.top(); ++i)
		cout << " ";
    }
}
