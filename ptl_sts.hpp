/*61:*/
#line 1581 "patlib.w"

#ifndef PTL_STS_H
#define PTL_STS_H

#include <map> 
#include <set> 

/*62:*/
#line 1609 "patlib.w"

template<class Texternal> 
class Simple_translation_service
{
public:
typedef unsigned Tinternal;
protected:
Texternal*to_external;
map<Texternal,Tinternal> to_internal;
Tinternal last_used_internal;

/*63:*/
#line 1632 "patlib.w"

public:
Simple_translation_service(const set<Texternal> &ext_set)
{
to_internal.clear();
last_used_internal= 0;
to_external= new Texternal[ext_set.size()+1];

for(set<Texternal> ::const_iterator i= ext_set.begin();
i!=ext_set.end();i++){
last_used_internal++;
to_external[last_used_internal]= *i;
to_internal.insert(make_pair(*i,last_used_internal));
}
}

virtual~Simple_translation_service()
{
delete[]to_external;
}

/*:63*/
#line 1620 "patlib.w"

/*64:*/
#line 1655 "patlib.w"

public:
inline virtual Tinternal get_last_used_internal(void)const
{
return last_used_internal;
}

/*:64*/
#line 1621 "patlib.w"

/*65:*/
#line 1669 "patlib.w"

public:
inline virtual Tinternal internal(const Texternal&e)const
{
map<Texternal,Tinternal> ::const_iterator it= to_internal.find(e);
return(*it).second;
}

inline virtual vector<Tinternal> internal(const vector<Texternal> &ve)const
{
vector<Tinternal> vi;

for(vector<Texternal> ::const_iterator i= ve.begin();i!=ve.end();
i++){
vi.push_back(internal(*i));
}
return vi;
}

/*:65*/
#line 1622 "patlib.w"

/*66:*/
#line 1692 "patlib.w"

public:
inline virtual Texternal external(const Tinternal&i)const
{
return to_external[i];
}

inline virtual vector<Texternal> external(const vector<Tinternal> &vi)const
{
vector<Texternal> ve;

for(vector<Tinternal> ::const_iterator i= vi.begin();i!=vi.end();
i++){
ve.push_back(external(*i));
}
return ve;
}

/*:66*/
#line 1623 "patlib.w"

};

/*:62*/
#line 1588 "patlib.w"


#endif

/*:61*/
