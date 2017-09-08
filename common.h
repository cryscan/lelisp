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

    std::map<atom, void*>::iterator begin() { return e.begin();}
    std::map<atom, void*>::iterator end() { return e.end(); }
    
    std::map<atom, void*>::const_iterator cbegin() { return e.cbegin();}
    std::map<atom, void*>::const_iterator cend() { return e.cend(); }

    std::map<atom, void*>::const_iterator find(atom a) { return e.find(a); }
    void* &operator[](atom a) { return e[a]; }
};

typedef void* (*prime_func)(env*, list*);
typedef std::map<prime_func, std::string> func_dict;

env* cons(env*);
void register_func(env*);
void* eval(env*, void*);
void clean(env*);

void* prime_quote(env*, list*);
void* prime_lambda(env*, list*);
void* prime_label(env*, list*);
void* prime_cond(env*, list*);
void* prime_defun(env*, list*);

void* prime_head(env*, list*);
void* prime_tail(env*, list*);
void* prime_cons(env*, list*);
void* prime_atom(env*, list*);
void* prime_eq(env*, list*);
void* prime_list(env*, list*);

/* running */
