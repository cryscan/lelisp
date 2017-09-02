#include <string>
#include "common.h"

using namespace std;

bool is_atom_char(char ch)
{
    return ch != '\0' &&
	ch != ' ' &&
	ch != '(' &&
	ch != ')';
}
bool is_blank(char ch)
{
    return ch == ' ' ||
	ch == '\t' ||
	ch == '\v' ||
	ch == '\r' ||
	ch == '\n';
}

string::const_iterator skip_blanks(string::const_iterator it)
{
    while (is_blank(*it))
	++it;
    return it;
}

parcel parse_atom(string::const_iterator it)
{
    string::const_iterator end = it;
    while (is_atom_char(*end))
	++end;
    return parcel(end, create_atom(string(it, end)));
}

parcel parse_list(string::const_iterator it)
{
    it = skip_blanks(it);
    if (*it == ')')
	return parcel(++it, 0); // return a nil list.
    
    parcel p_head = parse(it);
    parcel p_tail = parse_list(p_head.tail);
    return parcel(p_tail.tail, cons(p_head.content, (list*)p_tail.content));
}

parcel parse(string::const_iterator it)
{
    it = skip_blanks(it);
    if (*it == '(')
	return parse_list(++it);
    else if (*it == '\'')
    {
	parcel p = parse(++it);
	return parcel(p.tail, cons(atom_quote, cons(p.content, 0)));
    }
    else if (is_atom_char(*it))
	return parse_atom(it);
    return parcel(it, 0);
}
