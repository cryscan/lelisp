#include <string>
#include <map>
#include <unordered_map>
#include "common.h"

using namespace std;

map<string, atom> atom_dict;
unordered_set<atom> atom_set;
unordered_set<list*> list_set;

atom create_atom(const string& sym)
{
    if (atom_dict.find(sym) == atom_dict.end())
    {
	atom a = new string;
	*a = sym;
	atom_dict.insert({sym, a});
	atom_set.insert(a);
	return a;
    }
    else
	return atom_dict[sym];
}

list* cons(void* head, list* tail)
{
    list* l = new list;
    l->head = head;
    l->tail = tail;
    list_set.insert(l);
    return l;
}

void list_coll(unordered_set<list*>& set)
{ list_set = set; }

bool is_atom(void* ptr)
{ return atom_set.find((atom)ptr) != atom_set.end(); }
bool is_list(void* ptr)
{ return list_set.find((list*)ptr) != list_set.end(); }
