#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_set>
#include <functional>
#include <assert.h>

using namespace std;

typedef string* atom;
struct list
{
  void* head; // either an atom or a list.
  list* tail;
};

struct parcel
{
  string::const_iterator tail; // unparsed code.
  void* content; // parsed content.

  parcel(string::const_iterator t, void* c) : tail(t), content(c) {}
};

// containers.
typedef map<atom, void*> env;
map<string, atom> atom_dict;
unordered_set<atom> atom_set;
unordered_set<list*> list_set;

atom atom_true, atom_lambda, atom_quote;

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

bool is_atom(void* ptr)
{ return atom_set.find((atom)ptr) != atom_set.end(); }
 bool is_list(void* ptr)
{ return list_set.find((list*)ptr) != list_set.end(); }

/* parser. */
parcel parse_list(string::const_iterator);
parcel parse_atom(string::const_iterator);
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

/* evaluate.*/
typedef void* (*prime_func)(env*, list*);
void* eval(env*, void*);

void* prime_quote(env* e, list* l) // ('(...))
{ return l->head; } // don't eval the expr inside.
void* prime_lambda(env* e, list* l) // (lambda (p1...pn) e)
{ return cons(atom_lambda, l); }
void* prime_label(env* e, list* l) // (label f (lambda (p1...pn) e)
{
  if (is_atom(l->head))
    {
      atom label = atom(l->head);
      (*e)[label] = eval(e, l->tail->head); // ...
    }
  return 0;
}
void* prime_cond(env* e, list* l) // (cond ((...) (...)) ...)
{
  list* args = l;
  while (args != 0)
    {
      assert(!is_atom(args->head)); // syntax error.
      list* pair = (list*)(args->head);
      if (eval(e, pair->head) != 0)
	return eval(e, pair->tail->head);
      args = args->tail;
    }
  return 0;
}
void* prime_defun(env* e, list* l) // (defun f (p1...pn) e)
{
  list* lambda = cons(atom_lambda, l->tail);
  void* label = l->head;
  return prime_label(e, cons(label, cons(lambda, 0)));
}

void* prime_head(env* e, list* l)
{
  if (l->head && !is_atom(l->head))
    return ((list*)(l->head))->head;
  return 0;
}
void* prime_tail(env* e, list* l)
{
  if (l->head && !is_atom(l->head))
    return ((list*)(l->head))->tail;
  return 0;
}
void* prime_cons(env* e, list* l)
{ return cons(l->head, (list*)(l->tail->head)); }
void* prime_atom(env* e, list* l)
{
  if (l->head == 0 ||    // nil list.
      is_atom(l->head)) // or an atom.
    return atom_true;
  return 0;
}
void* prime_eq(env* e, list* l)
{
  void* fst = l->head;
  void* snd = l->tail->head;
  if ((fst == 0 && snd == 0) ||
      (is_atom(fst) && is_atom(snd) && fst == snd))
    return atom_true;
  return 0;
}
void* prime_list(env* e, list* l)
{ return l; }

map<prime_func, string> expr_func_dict =
  {
    {prime_quote, "quote"},
    {prime_lambda, "lambda"},
    {prime_label, "label"},
    {prime_cond, "cond"},
    {prime_defun, "defun"}
  };
map<prime_func, string> val_func_dict =
  {
    {prime_head, "car"},
    {prime_tail, "cdr"},
    {prime_cons, "cons"},
    {prime_atom, "atom"},
    {prime_eq, "eq"},
    {prime_list, "list"},
  };

bool is_expr_func(void* ptr)
{ return expr_func_dict.find((prime_func)ptr) != expr_func_dict.end(); }
bool is_val_func(void* ptr)
{ return val_func_dict.find((prime_func)ptr) != val_func_dict.end(); }

void register_func(env *e, map<prime_func, string> func_dict)
{
  for (auto p : func_dict)
    (*e)[create_atom(p.second)] = (void*)p.first;
}

void* eval_val(env* e, atom a)
{
  env::const_iterator it = e->find(a);
  if (it != e->cend())
    return it->second;
  return 0;
}

list* eval_args(env* e, list* l)
{
  if (l == 0)
    return 0;
  return cons(eval(e, l->head), eval_args(e, l->tail));
}

void* eval_expr(env* e, list* l)
{
  if (l == 0)
    return 0;

  void* v = eval(e, l->head);
  if (v != 0)
    {
      if (is_expr_func(v))
	{
	  prime_func f = *(prime_func)v;
	  return f(e, l->tail);
	}
      else if (is_val_func(v))
	{
	  prime_func f = *(prime_func)v;
	  return f(e, eval_args(e, l->tail));
	}
      else if (is_list(v))
	{
	  list* t = (list*)v;
	  atom head = atom(t->head);
	  if (head == atom_lambda) // l : ((lambda (p1...pn) e) a1...an)
	    {
	      env te = *e;
	      list* tail = t->tail; // ((p1...pn) e)
	      list* params = (list*)(tail->head); // (p1...pn)
	      void* body = tail->tail->head;
	      list* args = eval_args(&te, l->tail); // (a1...an)
	      while (params) // bind p to a.
		{
		  assert(is_atom(params->head)); // param error.
		  atom p = atom(params->head);
		  te[p] = args->head;
		  params = params->tail;

		  assert(args); // params don't match.
		  args = args->tail;
		}
	      return eval(&te, body);
	    }
	}
    }
  
  return 0;
}

void* eval(env* e, void* v)
{
  if (is_atom(v))
    return eval_val(e, (atom)v);
  else
    return eval_expr(e, (list*)v);
  return 0;
}

void clear(env* e)
{
  for (auto it = e->begin(); it != e->end();)
    {
      if (!is_list(it->second))
	e->erase(it++);
      else
	++it;
    }
}

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

void run(env* e)
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
	  line = line + " " + next;
	}
      parcel r = parse(line.cbegin());
      if (r.content == 0)
	continue;
      print(eval(e, (list*)r.content));
      cout << endl;
    }
}

int main(int argc, char** argv)
{
  env* e = new env;
  atom_true = create_atom("t");
  atom_lambda = create_atom("lambda");
  atom_quote = create_atom("quote");

  register_func(e, expr_func_dict);
  register_func(e, val_func_dict);
  
  run(e);
  
  return 0;
}
