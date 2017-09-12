#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>

/* common */
typedef std::string* atom;
struct list
{
    void* head;
    list* tail;
};

extern atom atom_true, atom_lambda, atom_quote;
atom create_atom(const std::string&);
list* cons(void*, list*);
void list_coll(std::unordered_set<list*>&);

bool is_atom(void*);
bool is_list(void*);

/* parser */
struct parcel
{
    std::string::const_iterator tail;
    void* content;
    
parcel(std::string::const_iterator it, void* v)
: tail(it), content(v) {}
};

parcel parse(std::string::const_iterator);

/* evaluate */
struct env
{
    std::map<atom, void*> e;
    env* prev;
    
    void* &operator [](atom a) { return e[a]; }
    
    auto begin() { return e.begin();}
    auto end() { return e.end(); }
    auto cbegin() { return e.cbegin();}
    auto cend() { return e.cend(); }
    auto find(atom a) { return e.find(a); }
};

typedef void* (*prime_func)(env*, list*);
typedef std::map<prime_func, std::string> func_dict;

env* cons(env*);
void register_func(env*);
void* eval(env*, void*);
void clean(env*);

/* interface */
void set_mode(int);
void execute(env*, std::istream&);
