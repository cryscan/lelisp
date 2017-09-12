#include <iostream>
#include <string>
#include <vector>
#include "common.h"

using namespace std;

typedef void (*callback)(void);
vector<int> paren_stack;

bool paren_matched(const string& str)
{
    for (int i = 0; i < str.length(); ++i)
    {
	const char ch = str[i];
	if (ch == '(')
	    paren_stack.push_back(i);
	if (ch == ')')
	    paren_stack.pop_back();
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
	    cout << "  ";
    }
}
