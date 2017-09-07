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

typedef std::map<atom, void*> env;

extern atom atom_true, atom_lambda, atom_quote;
atom create_atom(const std::string&);
list* cons(void*, list*);
void reset_list_set(std::unordered_set<list*>&);

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
typedef void* (*prime_func)(env*, list*);
typedef std::map<prime_func, std::string> func_dict;

void* eval(env*, void*);
void clean(env*);
void register_func(env*);

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
