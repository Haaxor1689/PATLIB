/*79:*/
#line 2003 "patlib.w"

#ifndef PTL_GEN_H
#define PTL_GEN_H

#include <iostream> 
#include <cstdio> 
#include <iomanip> 
#include "ptl_exc.h"
#include "ptl_mopm.h"
#include "ptl_hwrd.h"

/*80:*/
#line 2025 "patlib.w"

template<class Tindex,class Tin_alph,class Tval_type,class Twt_type,
class Tcount_type,class THword,class TTranslate,
class TCandidate_count_structure,
class TCompetitive_multi_out_pat_manip,
class TOutputs_of_a_pattern,
class TWord_input_file> 
class Pass
{
/*81:*/
#line 2048 "patlib.w"

protected:
TTranslate&translate;
const Tval_type hyph_level;
const Tval_type hopeless_hyph_val;
const Tindex left_hyphen_min;
const Tindex right_hyphen_min;
const Tindex pat_len;
const Tindex pat_dot;
const Tcount_type good_wt,bad_wt,thresh;
TCompetitive_multi_out_pat_manip&patterns;


Tcount_type good_count,bad_count,miss_count;
TCandidate_count_structure candidates;

TWord_input_file word_input;


Tindex hyf_min,hyf_max,hyf_len;
Tindex dot_min,dot_max,dot_len;
typename THword::Thyf_type good_dot,bad_dot;


/*:81*/
#line 2034 "patlib.w"

/*82:*/
#line 2084 "patlib.w"

public:
Pass(TTranslate&tra,const char*i_d_f_n,
const Tval_type&l,const Tval_type&h,
const Tindex&lhm,const Tindex&rhm,
const Tindex&p_l,const Tindex&p_d,
const Twt_type&g_w,const Twt_type&b_w,const Twt_type&t,
TCompetitive_multi_out_pat_manip&pat):
translate(tra),
hyph_level(l),hopeless_hyph_val(h),
left_hyphen_min(lhm),right_hyphen_min(rhm),
pat_len(p_l),pat_dot(p_d),
good_wt(g_w),bad_wt(b_w),thresh(t),
patterns(pat),
good_count(0),bad_count(0),miss_count(0),
candidates(patterns.get_max_in_alph(),0,0),
word_input(translate,i_d_f_n)
{
hyf_min= left_hyphen_min+1;
hyf_max= right_hyphen_min+1;
hyf_len= hyf_min+hyf_max;
dot_min= pat_dot;
dot_max= pat_len-pat_dot;
if(dot_min<hyf_min)dot_min= hyf_min;
if(dot_max<hyf_max)dot_max= hyf_max;
dot_len= dot_min+dot_max;
if(hyph_level%2==1){
good_dot= THword::is_hyf;
bad_dot= THword::no_hyf;
}else{
good_dot= THword::err_hyf;
bad_dot= THword::found_hyf;
}
}

/*:82*/
#line 2035 "patlib.w"


/*83:*/
#line 2132 "patlib.w"

public:
void hyphenate(THword&w)
{
TOutputs_of_a_pattern o;
typename TOutputs_of_a_pattern::iterator i;
patterns.competitive_pattern_output(w,o,hopeless_hyph_val);

for(i= o.begin();i!=o.end();i++){
w.hval[i->first]= i->second;
}

patterns.competitive_pattern_output(w,o,hopeless_hyph_val+1);

for(i= o.begin();i!=o.end();i++){
if(i->second>=hyph_level&&
i->first>=dot_min&&i->first<=w.size()-dot_max){

vector<Tin_alph> subw;
for(Tindex j= i->first+1-pat_dot;
j<=i->first+pat_len-pat_dot;j++){
subw.push_back(w[j]);
}
TOutputs_of_a_pattern subwo;
patterns.competitive_pattern_output(subw,subwo,
hopeless_hyph_val+1);
typename TOutputs_of_a_pattern::iterator val_on_pat_dot= 
subwo.find(pat_dot);


if(val_on_pat_dot!=subwo.end())
if(val_on_pat_dot->second>=hyph_level)
w.no_more[i->first]= true;
}
}
}

/*:83*/
#line 2037 "patlib.w"

/*84:*/
#line 2180 "patlib.w"

public:
void change_dots(THword&w)
{
for(Tindex i= w.size()-hyf_max;i>=hyf_min;i--){
if(w.hval[i]%2==1){
if(w.dots[i]==THword::no_hyf)
w.dots[i]= THword::err_hyf;
else if(w.dots[i]==THword::is_hyf)
w.dots[i]= THword::found_hyf;
}

if(w.dots[i]==THword::found_hyf)
good_count+= w.dotw[i];
else if(w.dots[i]==THword::err_hyf)
bad_count+= w.dotw[i];
else if(w.dots[i]==THword::is_hyf)
miss_count+= w.dotw[i];
}
}

/*:84*/
#line 2038 "patlib.w"

/*85:*/
#line 2212 "patlib.w"

protected:
void do_word(THword&w)
{
for(Tindex dpos= w.size()-dot_max;dpos>=dot_min;dpos--){
/*86:*/
#line 2238 "patlib.w"

if(w.no_more[dpos])continue;
if((w.dots[dpos]!=good_dot)&&(w.dots[dpos]!=bad_dot))continue;

/*:86*/
#line 2217 "patlib.w"


Tindex spos= dpos-pat_dot;
Tindex fpos= spos+pat_len;
spos++;

vector<Tin_alph> subw;
for(Tindex i= spos;i<=fpos;i++)subw.push_back(w[i]);

if(w.dots[dpos]==good_dot){
candidates.increment_counts(subw,w.dotw[dpos],0);
}
else{
candidates.increment_counts(subw,0,w.dotw[dpos]);
}
}
}

/*:85*/
#line 2039 "patlib.w"

/*87:*/
#line 2244 "patlib.w"

public:
void print_pass_statistics(void)
{
cout<<endl;
cout<<good_count<<" good "<<bad_count<<" bad "<<miss_count<<" missed"<<endl;
if(good_count+miss_count> 0){
cout<<100.0*good_count/float(good_count+miss_count)<<" % ";
cout<<100.0*bad_count/float(good_count+miss_count)<<" % ";
cout<<100.0*miss_count/float(good_count+miss_count)<<" % "<<endl;
}
}

/*:87*/
#line 2040 "patlib.w"

/*88:*/
#line 2260 "patlib.w"

protected:
void do_dictionary(void)
{
THword w;
while(word_input.get(w)){
if(w.size()>=hyf_len){
hyphenate(w);
change_dots(w);
}
if(w.size()>=dot_len)do_word(w);
}

#ifdef DEBUG

TOutputs_of_a_pattern o;
vector<Tin_alph> word;
patterns.init_walk_through();
cout<<"Patterns in the pattern manipulator:"<<endl;
while(patterns.get_next_pattern(word,o)){
cout<<"Word ";
for(vector<Tin_alph> ::iterator i= word.begin();i!=word.end();
i++)cout<<*i<<" ";
cout<<"... has ";
for(typename TOutputs_of_a_pattern::const_iterator i= o.begin();
i!=o.end();i++)
cout<<"("<<i->first<<","<<i->second<<") ";
cout<<endl;
}
vector<Tin_alph> vect;
Tcount_type a;Tcount_type b;
candidates.init_walk_through();
cout<<"Candidates:"<<endl;
while(candidates.get_next_pattern(vect,a,b)){
for(vector<Tin_alph> ::iterator i= vect.begin();i!=vect.end();i++)
cout<<*i<<" ";
cout<<"with g,b: "<<a<<","<<b<<endl;
}

#endif

print_pass_statistics();
cout<<"Count data structure statistics:"<<endl;
candidates.print_statistics();
}

/*:88*/
#line 2041 "patlib.w"

/*89:*/
#line 2322 "patlib.w"

protected:
bool collect_candidates(Tcount_type&level_pattern_count)
{
cout<<"Collecting candidates"<<endl;

bool more_to_come= false;
vector<Tin_alph> w;
Tcount_type g,b;
Tcount_type good_pat_count= 0;
Tcount_type bad_pat_count= 0;


candidates.init_walk_through();
while(candidates.get_next_pattern(w,g,b)){
if(good_wt*g<thresh){
patterns.insert_pattern(w,pat_dot,hopeless_hyph_val);
bad_pat_count++;
}
else{


if(good_wt*g>=thresh+bad_wt*b){
patterns.insert_pattern(w,pat_dot,hyph_level,1);

good_pat_count++;
good_count+= g;
bad_count+= b;
}
else more_to_come= true;
}
}

cout<<good_pat_count<<" good and "<<bad_pat_count<<" bad patterns added";
if(more_to_come==true)cout<<" (more to come)";
cout<<endl;
cout<<"finding "<<good_count<<" good and "<<bad_count<<" bad hyphens"<<endl;
if(good_pat_count> 0){
cout<<"efficiency = "
<<float(good_count)/(float(good_pat_count)+float(bad_count)/
(float(thresh)/float(good_wt)))
<<endl;
}
cout<<"Pattern data structure statistics:"<<endl;
patterns.print_statistics();

level_pattern_count+= good_pat_count;
return more_to_come;
}

/*:89*/
#line 2042 "patlib.w"

/*90:*/
#line 2374 "patlib.w"

public:
bool do_all(Tcount_type&level_pattern_count)
{
cout<<endl<<"Generating a pass with pat_len = "<<pat_len
<<", pat_dot = "<<pat_dot<<endl;

do_dictionary();
return collect_candidates(level_pattern_count);
}

/*:90*/
#line 2043 "patlib.w"

};

/*:80*/
#line 2014 "patlib.w"

/*91:*/
#line 2389 "patlib.w"

template<class Tindex,class Tin_alph,class Tval_type,class Twt_type,
class Tcount_type,class THword,class TTranslate,
class TCandidate_count_structure,
class TCompetitive_multi_out_pat_manip,
class TWord_input_file,class TPass> 
class Level
{
/*92:*/
#line 2417 "patlib.w"

protected:
TTranslate&translate;
const char*word_input_file_name;
const Tval_type hyph_level;
const Tval_type hopeless_hyph_val;
const Tindex left_hyphen_min,right_hyphen_min;
TCompetitive_multi_out_pat_manip&patterns;
Tcount_type level_pattern_count;
Tindex pat_start,pat_finish;
Tcount_type good_wt,bad_wt,thresh;

/*:92*/
#line 2397 "patlib.w"

/*93:*/
#line 2432 "patlib.w"

public:
Level(TTranslate&tra,const char*i_d_f_n,
const Tval_type&l,const Tval_type&h,
const Tindex&lhm,const Tindex&rhm,
TCompetitive_multi_out_pat_manip&p):
translate(tra),word_input_file_name(i_d_f_n),
hyph_level(l),hopeless_hyph_val(h),
left_hyphen_min(lhm),right_hyphen_min(rhm),
patterns(p),
level_pattern_count(0)
{
do{
cout<<"pat_start, pat_finish: ";
cin>>pat_start;cin>>pat_finish;
if(pat_start<1||pat_start> pat_finish){
cout<<"Specify two integers satisfying 1<=pat_start<=pat_finish ";
pat_start= 0;
}
}while(pat_start<1);
do{
cout<<"good weight, bad weight, threshold: ";
cin>>good_wt;cin>>bad_wt;cin>>thresh;
if(good_wt<1||bad_wt<1||thresh<1){
cout<<"Specify three integers: good weight, bad weight, threshold>=1 ";
good_wt= 0;
}
}while(good_wt<1);
}

/*:93*/
#line 2398 "patlib.w"

/*94:*/
#line 2474 "patlib.w"

public:
void do_all(void)
{
cout<<endl<<endl<<"Generating level "<<hyph_level<<endl;
Growing_array<Tindex,char> more_this_level(true);
for(Tindex pat_len= pat_start;pat_len<=pat_finish;pat_len++){

Tindex pat_dot= pat_len/2;
Tindex dot1= pat_dot*2;
do{
pat_dot= dot1-pat_dot;dot1= pat_len*2-dot1-1;
if(more_this_level[pat_dot]){
TPass pass(translate,word_input_file_name,
hyph_level,hopeless_hyph_val,
left_hyphen_min,right_hyphen_min,
pat_len,pat_dot,good_wt,
bad_wt,thresh,patterns);
more_this_level[pat_dot]= pass.do_all(level_pattern_count);
}
}while(pat_dot!=pat_len);
for(Tindex k= more_this_level.size();k>=1;k--)
if(more_this_level[k-1]!=true)
more_this_level[k]= false;
}

Tindex old_p_c= patterns.get_pat_count();
patterns.delete_values(hopeless_hyph_val);
cout<<old_p_c-patterns.get_pat_count()
<<" bad patterns deleted"<<endl;
patterns.delete_hanging();

cout<<"total of "<<level_pattern_count<<" patterns at level "
<<hyph_level<<endl;
}

/*:94*/
#line 2399 "patlib.w"

};

/*:91*/
#line 2015 "patlib.w"

/*95:*/
#line 2514 "patlib.w"

template<class Tindex,class Tin_alph,class Tval_type,class Twt_type,
class Tcount_type,class THword,class TTranslate,
class TCandidate_count_structure,
class TCompetitive_multi_out_pat_manip,
class TOutputs_of_a_pattern,
class TWord_input_file,class TWord_output_file,
class TPattern_input_file,class TPattern_output_file,
class TPass,class TLevel> 
class Generator
{
/*96:*/
#line 2537 "patlib.w"

protected:
TTranslate translate;
const char*name;
const char*word_input_file_name;
const char*pattern_input_file_name;
const char*pattern_output_file_name;
TCompetitive_multi_out_pat_manip patterns;
Tval_type hyph_start,hyph_finish;
Tindex left_hyphen_min,right_hyphen_min;

/*:96*/
#line 2525 "patlib.w"

/*97:*/
#line 2551 "patlib.w"

public:
Generator(const char*dic,const char*pat,
const char*out,const char*tra):
translate(tra),word_input_file_name(dic),
pattern_input_file_name(pat),
pattern_output_file_name(out),
patterns(translate.get_max_in_alph()),
left_hyphen_min(translate.get_left_hyphen_min()),
right_hyphen_min(translate.get_right_hyphen_min())
{
do{
cout<<"hyph_start, hyph_finish: ";
cin>>hyph_start;cin>>hyph_finish;
if((hyph_start<1)||(hyph_finish<1)){
hyph_start= 0;
cout<<"Specify two integers satisfying 1<=hyph_start, hyph_finish "
<<endl;
}
}while(hyph_start<1);
}

/*:97*/
#line 2526 "patlib.w"

/*98:*/
#line 2575 "patlib.w"

void read_patterns(void)
{
vector<Tin_alph> v;
TOutputs_of_a_pattern o;

TPattern_input_file file(translate,pattern_input_file_name);

while(file.get(v,o)){
if(v.size()> 0){
for(typename TOutputs_of_a_pattern::iterator i= o.begin();
i!=o.end();i++){
if(i->second>=hyph_start){
throw Patlib_error("! The patterns to be read in contain "
"hyphenation value bigger than hyph_start.");
}
patterns.insert_pattern(v,i->first,i->second);
}
}
}
}

/*:98*/
#line 2527 "patlib.w"

/*99:*/
#line 2599 "patlib.w"

void output_patterns(void)
{
TPattern_output_file file(translate,pattern_output_file_name);

vector<Tin_alph> v;
TOutputs_of_a_pattern o;
patterns.init_walk_through();
while(patterns.get_next_pattern(v,o)){
file.put(v,o);
}
}

/*:99*/
#line 2528 "patlib.w"

/*100:*/
#line 2631 "patlib.w"

public:
void hyphenate_word_list(void)
{
string s;
cout<<"hyphenate word list <y/n>? ";cin>>s;
if(!(s=="y"||s=="Y"))return;

Tval_type level_value= hyph_finish;
if(hyph_start> hyph_finish)level_value= hyph_start;
Tval_type fake_level_value= 2*((level_value/2)+1);

char file_name[100];
sprintf(file_name,"pattmp.%d",level_value);
cout<<"Writing file "<<file_name<<endl;

THword w;
TWord_output_file o_f(translate,file_name);
TWord_input_file i_f(translate,word_input_file_name);
TPass pass(translate,word_input_file_name,
level_value,fake_level_value,
left_hyphen_min,right_hyphen_min,
1,1,1,1,1,patterns);
while(i_f.get(w)){
if(w.size()> 2){
pass.hyphenate(w);
pass.change_dots(w);
}
o_f.put(w);
}
pass.print_pass_statistics();
}

/*:100*/
#line 2529 "patlib.w"

/*101:*/
#line 2668 "patlib.w"

public:
void do_all(void)
{
read_patterns();
cout<<patterns.get_pat_count()<<" pattern lines read in"<<endl;

Tval_type hopeless_fake_number= 2*((hyph_finish/2)+1);

for(Tval_type l= hyph_start;l<=hyph_finish;l++){
TLevel level(translate,word_input_file_name,
l,hopeless_fake_number,left_hyphen_min,
right_hyphen_min,patterns);
level.do_all();
}
output_patterns();
hyphenate_word_list();
cout<<endl;
}/*:101*/
#line 2530 "patlib.w"

};

/*:95*/
#line 2016 "patlib.w"


#endif

/*:79*/
