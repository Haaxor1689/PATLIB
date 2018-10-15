/*10:*/
#line 164 "patlib.w"

#ifndef PTL_GA_H 
#define PTL_GA_H

#include <iostream> 
#include <vector> 
#include "ptl_exc.h"

/*11:*/
#line 192 "patlib.w"

template<class Tindex,class Tmember> 
class Growing_array:
public std::vector<Tmember> 
{
protected:
const Tmember default_value;
/*12:*/
#line 206 "patlib.w"

public:
Growing_array(const Tmember&def_val):
std::vector<Tmember> (),default_value(def_val)
{
}

/*:12*/
#line 199 "patlib.w"

/*13:*/
#line 216 "patlib.w"

public:
inline Tmember&operator[](const Tindex&logical_addr)
{
try{
while(logical_addr>=std::vector<Tmember> ::size())
std::vector<Tmember> ::push_back(default_value);
return std::vector<Tmember> ::operator[](logical_addr);
}
catch(...){
cerr<<endl<<"! Growing array: error in [] operator"<<endl;
cerr<<"  Logical member requested "<<logical_addr<<endl;
cerr<<"  No. of members used      "<<std::vector<Tmember> ::size()<<endl;
cerr<<"  No. of members reserved  "<<std::vector<Tmember> ::capacity()<<
endl;
cerr<<"  Member size (bytes)      "<<sizeof(Tmember)<<endl;
throw Patlib_error("");
}
}

/*:13*/
#line 200 "patlib.w"

/*14:*/
#line 238 "patlib.w"

public:
void print_statistics(void)const
{
cout<<"  No. of members used       "<<std::vector<Tmember> ::size()<<endl;
cout<<"  No. of members reserved   "<<std::vector<Tmember> ::capacity()<<endl;
cout<<"  Member size (bytes)       "<<sizeof(Tmember)<<endl;
}

/*:14*/
#line 201 "patlib.w"

};

/*:11*/
#line 172 "patlib.w"


#endif

/*:10*/
