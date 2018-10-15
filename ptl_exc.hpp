/*4:*/
#line 94 "patlib.w"

#ifndef PTL_EXC_H
#define PTL_EXC_H
#include <iostream> 
#include <string> 
#include <exception> 

class Patlib_error:
public std::string{
public:
Patlib_error(string s):
std::string(s){
}
void what(void){
cout<<*this;
}
};

#endif

/*:4*/
