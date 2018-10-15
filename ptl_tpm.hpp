/*15:*/
#line 251 "patlib.w"

#ifndef PTL_TPM_H
#define PTL_TPM_H

#include <iostream> 
#include <vector> 
#include <set> 
#include "ptl_exc.h"
#include "ptl_ga.h"

/*16:*/
#line 317 "patlib.w"

template<class Tpm_pointer,class Tin_alph,class Tout_information> 
class Trie_pattern_manipulator
{
/*17:*/
#line 363 "patlib.w"

protected:
enum{
min_in_alph= 0
};
enum{
trie_root= 1
};


Tpm_pointer trie_max;
Tpm_pointer trie_bmax;
Tpm_pointer trie_count;
Tpm_pointer pat_count;

const Tin_alph max_in_alph;
const Tout_information out_inf_zero;

Growing_array<Tpm_pointer,Tin_alph> trie_char;
Growing_array<Tpm_pointer,Tpm_pointer> trie_link;
Growing_array<Tpm_pointer,Tpm_pointer> trie_back;
Growing_array<Tpm_pointer,char> trie_base_used;


Growing_array<Tpm_pointer,Tout_information> trie_outp;

unsigned q_max;
unsigned q_max_thresh;
Tin_alph*trieq_char;
Tpm_pointer*trieq_link;
Tpm_pointer*trieq_back;
Tout_information*trieq_outp;

/*:17*/
#line 321 "patlib.w"

/*18:*/
#line 400 "patlib.w"

public:
virtual unsigned get_q_max_thresh(void)const
{
return q_max_thresh;
}

virtual void set_q_max_thresh(const unsigned&new_q_m_t)
{

if(new_q_m_t> 0)q_max_thresh= new_q_m_t;
}

virtual Tpm_pointer get_trie_count(void)const

{
return trie_count;
}

virtual Tpm_pointer get_pat_count(void)const

{
return pat_count;
}

virtual Tpm_pointer get_max_in_alph(void)const

{
return max_in_alph;
}

/*:18*/
#line 322 "patlib.w"


/*19:*/
#line 436 "patlib.w"

public:
Trie_pattern_manipulator(const Tin_alph&max_i_a,
const Tout_information&out_i_z,
const unsigned&q_thr= 5):

max_in_alph(max_i_a),out_inf_zero(out_i_z),
trie_char(min_in_alph),trie_link(min_in_alph),
trie_back(min_in_alph),trie_base_used(false),trie_outp(out_inf_zero),
q_max_thresh(q_thr)
{
for(Tpm_pointer c= min_in_alph;c<=max_in_alph;c++){
trie_char[trie_root+c]= c;
}





trie_base_used[trie_root]= true;trie_bmax= trie_root;
trie_max= trie_root+max_in_alph;

trie_count= max_in_alph+1;
pat_count= 0;

trie_link[0]= trie_max+1;
trie_back[trie_max+1]= 0;

trieq_char= new Tin_alph[max_in_alph+1];
trieq_link= new Tpm_pointer[max_in_alph+1];
trieq_back= new Tpm_pointer[max_in_alph+1];
trieq_outp= new Tout_information[max_in_alph+1];
}

/*:19*/
#line 324 "patlib.w"

/*20:*/
#line 472 "patlib.w"

public:
virtual~Trie_pattern_manipulator()
{
delete[]trieq_char;
delete[]trieq_link;
delete[]trieq_back;
delete[]trieq_outp;
}

/*:20*/
#line 325 "patlib.w"


/*21:*/
#line 489 "patlib.w"

protected:
virtual Tpm_pointer first_fit(void)
{
unsigned int q;
Tpm_pointer s,t;
/*22:*/
#line 517 "patlib.w"

if(q_max> q_max_thresh){
t= trie_back[trie_max+1];
}else{t= 0;}
while(1){
t= trie_link[t];
s= t-trieq_char[1];
/*23:*/
#line 543 "patlib.w"

while(trie_bmax<s){
trie_bmax++;trie_base_used[trie_bmax]= false;
trie_char[trie_bmax+max_in_alph]= min_in_alph;
trie_outp[trie_bmax+max_in_alph]= out_inf_zero;

trie_link[trie_bmax+max_in_alph]= trie_bmax+max_in_alph+1;
trie_back[trie_bmax+max_in_alph+1]= trie_bmax+max_in_alph;
}

/*:23*/
#line 525 "patlib.w"

if(trie_base_used[s]){
goto not_found;
}
for(q= q_max;q>=2;q--){
if(trie_char[s+trieq_char[q]]!=min_in_alph){
goto not_found;
}
}
goto found;
not_found:;
}
found:;

/*:22*/
#line 496 "patlib.w"


for(q= 1;q<=q_max;q++){
t= s+trieq_char[q];

trie_link[trie_back[t]]= trie_link[t];
trie_back[trie_link[t]]= trie_back[t];
trie_char[t]= trieq_char[q];
trie_link[t]= trieq_link[q];
trie_back[t]= trieq_back[q];
trie_outp[t]= trieq_outp[q];
if(t> trie_max){trie_max= t;}
}
trie_base_used[s]= true;
return s;
}

/*:21*/
#line 327 "patlib.w"

/*24:*/
#line 558 "patlib.w"

protected:
virtual void unpack(const Tpm_pointer&s)
{
Tpm_pointer t;

q_max= 1;
for(Tpm_pointer c= min_in_alph;c<=max_in_alph;c++){
t= s+c;
if(trie_char[t]==c&&c!=min_in_alph){

trieq_char[q_max]= c;
trieq_link[q_max]= trie_link[t];
trieq_back[q_max]= trie_back[t];
trieq_outp[q_max]= trie_outp[t];
q_max++;

trie_back[trie_link[0]]= t;trie_link[t]= trie_link[0];
trie_link[0]= t;trie_back[t]= 0;
trie_char[t]= min_in_alph;
trie_outp[t]= out_inf_zero;
}
}
trie_base_used[s]= false;
}

/*:24*/
#line 328 "patlib.w"


/*25:*/
#line 592 "patlib.w"

public:
virtual void hard_insert_pattern(const vector<Tin_alph> &w,
const Tout_information&o)
{
if(w.empty())return;


Tpm_pointer s,t;
vector<Tin_alph> ::const_iterator i= w.begin();
s= trie_root+*i;
t= trie_link[s];

while((t> 0)&&((i+1)!=w.end())){

i++;
t+= *i;
if(trie_char[t]!=*i){
/*26:*/
#line 646 "patlib.w"

if(trie_char[t]==min_in_alph){

trie_link[trie_back[t]]= trie_link[t];
trie_back[trie_link[t]]= trie_back[t];
trie_char[t]= *i;
trie_link[t]= 0;
trie_back[t]= 0;
trie_outp[t]= out_inf_zero;
if(t> trie_max){
trie_max= t;
}
}
else{

unpack(t-*i);
trieq_char[q_max]= *i;
trieq_link[q_max]= 0;
trieq_back[q_max]= 0;
trieq_outp[q_max]= out_inf_zero;
t= first_fit();
trie_link[s]= t;
t+= *i;
}
trie_count++;

/*:26*/
#line 611 "patlib.w"

}
s= t;
t= trie_link[s];
}
trieq_link[1]= 0;
trieq_back[1]= 0;
trieq_outp[1]= out_inf_zero;
q_max= 1;
while((i+1)!=w.end()){

i++;
trieq_char[1]= *i;
t= first_fit();
trie_link[s]= t;
s= t+*i;
trie_count++;
}

if((trie_outp[s]==out_inf_zero)&&(o!=out_inf_zero)){

pat_count++;
}
if((trie_outp[s]!=out_inf_zero)&&(o==out_inf_zero)){

pat_count--;
}

trie_outp[s]= o;
}

/*:25*/
#line 330 "patlib.w"

/*27:*/
#line 678 "patlib.w"

protected:
vector<Tpm_pointer> pointer_stack;
vector<Tpm_pointer> char_stack;

public:
virtual void init_walk_through()
{
pointer_stack.clear();
char_stack.clear();
pointer_stack.push_back(trie_root);
char_stack.push_back(min_in_alph);
}

/*:27*/
#line 331 "patlib.w"

/*28:*/
#line 708 "patlib.w"

public:
virtual bool get_next_pattern(vector<Tin_alph> &w,
Tout_information&o)
{
Tpm_pointer t,tstart;
Tpm_pointer c,cstart;

w.clear();
o= out_inf_zero;

while(true){
if(pointer_stack.empty())return false;

tstart= pointer_stack.back();pointer_stack.pop_back();


cstart= char_stack.back();char_stack.pop_back();

for(c= cstart;c<=max_in_alph;c++){

t= tstart+c;

if(trie_char[t]==c&&c!=min_in_alph){

pointer_stack.push_back(tstart);
char_stack.push_back(c+1);
if(trie_outp[t]!=out_inf_zero){

/*30:*/
#line 768 "patlib.w"

vector<Tpm_pointer> ::iterator it= pointer_stack.begin();
vector<Tpm_pointer> ::iterator ic= char_stack.begin();
Tpm_pointer u;

for(Tpm_pointer i= 0;i<pointer_stack.size();i++){
u= (*it)+Tpm_pointer(*ic)-1;
w.push_back(trie_char[u]);
it++;ic++;
}
o= trie_outp[u];

/*:30*/
#line 738 "patlib.w"


/*29:*/
#line 754 "patlib.w"

if(trie_link[t]!=0){

pointer_stack.push_back(trie_link[t]);
char_stack.push_back(min_in_alph);
}

/*:29*/
#line 741 "patlib.w"

return true;
}
/*29:*/
#line 754 "patlib.w"

if(trie_link[t]!=0){

pointer_stack.push_back(trie_link[t]);
char_stack.push_back(min_in_alph);
}

/*:29*/
#line 745 "patlib.w"

break;
}
}
}
}

/*:28*/
#line 332 "patlib.w"

/*31:*/
#line 795 "patlib.w"

public:
void word_output(const vector<Tin_alph> &w,
vector<Tout_information> &o)
{
Tpm_pointer t;
vector<Tin_alph> ::const_iterator i= w.begin();
o.clear();

if(w.empty())return;

t= trie_root;

do{
t+= *i;
if(trie_char[t]==*i)o.push_back(trie_outp[t]);
else break;
t= trie_link[t];
i++;
}while(t!=0&&i!=w.end());

while(i!=w.end()){
o.push_back(out_inf_zero);
i++;
}
}

/*:31*/
#line 333 "patlib.w"

/*32:*/
#line 829 "patlib.w"

public:
void word_last_output(const vector<Tin_alph> &w,
Tout_information&o)
{
#if 0==1 
o= out_inf_zero;

vector<Tout_information> whole_o;
word_output(w,whole_o);
if(whole_o.size()>=1)o= *(whole_o.end()-1);
#endif
Tpm_pointer s,t;
vector<Tin_alph> ::const_iterator i= w.begin();
o= out_inf_zero;

if(w.empty())return;

t= trie_root;


do{
t+= *i;
if(trie_char[t]==*i)s= t;
else break;
t= trie_link[t];
i++;
}while(t!=0&&i!=w.end());


if(i==w.end()){
o= trie_outp[s];
}
}

/*:32*/
#line 334 "patlib.w"

/*33:*/
#line 875 "patlib.w"

private:
virtual bool delete_hanging_level(const Tpm_pointer&s)
{
Tpm_pointer t;
bool all_freed;

all_freed= true;
for(Tpm_pointer c= min_in_alph;c<=max_in_alph;c++){

t= s+c;
if(trie_char[t]==c&&c!=min_in_alph){
if(trie_link[t]!=0){
if(delete_hanging_level(trie_link[t]))trie_link[t]= 0;
}
if((trie_link[t]!=0)||(trie_outp[t]!=out_inf_zero)||
(s==trie_root))all_freed= false;
else/*34:*/
#line 911 "patlib.w"

{
trie_link[trie_back[trie_max+1]]= t;
trie_back[t]= trie_back[trie_max+1];
trie_link[t]= trie_max+1;
trie_back[trie_max+1]= t;
trie_char[t]= min_in_alph;
trie_count--;
}

/*:34*/
#line 893 "patlib.w"

}
}
if(all_freed){
trie_base_used[s]= false;
}
return all_freed;
}

public:
virtual void delete_hanging(void)
{
delete_hanging_level(trie_root);
}

/*:33*/
#line 335 "patlib.w"

/*35:*/
#line 926 "patlib.w"

public:
void set_of_my_outputs(set<Tout_information> &s)
{
s.clear();
for(Tpm_pointer i= 0;i<=trie_max;i++){
s.insert(trie_outp[i]);
}
}

/*:35*/
#line 336 "patlib.w"

/*36:*/
#line 939 "patlib.w"

public:
void print_statistics(int detail= 0)const
{
cout<<"  nodes:                           "<<trie_count<<endl;
cout<<"  patterns:                        "<<pat_count<<endl;
cout<<"  trie_max:                        "<<trie_max<<endl;
cout<<"  current q_max_thresh:            "<<q_max_thresh<<endl;
if(detail){
cout<<"Trie char"<<endl;
trie_char.print_statistics();
cout<<"Trie link"<<endl;
trie_link.print_statistics();
cout<<"Trie back"<<endl;
trie_back.print_statistics();
cout<<"Trie base used"<<endl;
trie_base_used.print_statistics();
cout<<"Trie outp"<<endl;
trie_outp.print_statistics();
}
}

/*:36*/
#line 337 "patlib.w"

};

/*:16*/
#line 261 "patlib.w"

/*37:*/
#line 988 "patlib.w"

template<class Tpm_pointer,class Tin_alph,
class Tcount_good,class Tcount_bad> 
class Candidate_count_trie:
public Trie_pattern_manipulator<Tpm_pointer,Tin_alph,
pair<Tcount_good,Tcount_bad> > {
public:
typedef pair<Tcount_good,Tcount_bad> Tcount_pair;
/*38:*/
#line 1005 "patlib.w"

public:
Candidate_count_trie(const Tin_alph&max_i_a,
const Tcount_good&out_i_z_good,
const Tcount_bad&out_i_z_bad,
const unsigned&q_thr= 3):
Trie_pattern_manipulator<Tpm_pointer,Tin_alph,Tcount_pair> 
(max_i_a,make_pair(out_i_z_good,out_i_z_bad),q_thr)
{
}

/*:38*/
#line 996 "patlib.w"

/*39:*/
#line 1025 "patlib.w"

public:
virtual void increment_counts(const vector<Tin_alph> &w,
const Tcount_good&good_inc,
const Tcount_bad&bad_inc)
{
Tcount_pair counts;

word_last_output(w,counts);
counts.first+= good_inc;
counts.second+= bad_inc;
hard_insert_pattern(w,counts);

}

/*:39*/
#line 997 "patlib.w"

/*40:*/
#line 1045 "patlib.w"

public:
virtual bool get_next_pattern(vector<Tin_alph> &w,
Tcount_good&good,Tcount_bad&bad)
{
Tcount_pair counts;
bool ret_val;

ret_val= Trie_pattern_manipulator<Tpm_pointer,Tin_alph,
pair<Tcount_good,Tcount_bad> > ::get_next_pattern(w,counts);
good= counts.first;
bad= counts.second;
return ret_val;
}

/*:40*/
#line 998 "patlib.w"

};

/*:37*/
#line 262 "patlib.w"

#endif

/*:15*/
