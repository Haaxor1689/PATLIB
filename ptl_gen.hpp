#pragma once

#include <iostream>
#include <cstdio>
#include <iomanip>

#include "exception.hpp"
#include "ptl_mopm.hpp"
#include "hyphenated_word.hpp"

namespace ptl {

template <class Tindex, class Tin_alph, class Tval_type, class Twt_type,
          class Tcount_type, class THword, class TTranslate,
          class TCandidate_count_structure,
          class TCompetitive_multi_out_pat_manip,
          class TOutputs_of_a_pattern,
          class TWord_input_file>
class pass {

protected:
    TTranslate& translate;
    const Tval_type hyph_level;
    const Tval_type hopeless_hyph_val;
    const Tindex left_hyphen_min;
    const Tindex right_hyphen_min;
    const Tindex pat_len;
    const Tindex pat_dot;
    const Tcount_type good_wt, bad_wt, thresh;
    TCompetitive_multi_out_pat_manip& patterns;

    Tcount_type good_count, bad_count, miss_count;
    TCandidate_count_structure candidates;

    TWord_input_file word_input;

    Tindex hyf_min, hyf_max, hyf_len;
    Tindex dot_min, dot_max, dot_len;
    hyphenation_type good_dot, bad_dot;

public:
    pass(TTranslate& tra, const char* i_d_f_n,
         const Tval_type& l, const Tval_type& h,
         const Tindex& lhm, const Tindex& rhm,
         const Tindex& p_l, const Tindex& p_d,
         const Twt_type& g_w, const Twt_type& b_w, const Twt_type& t,
         TCompetitive_multi_out_pat_manip& pat):
        translate(tra),
        hyph_level(l), hopeless_hyph_val(h),
        left_hyphen_min(lhm), right_hyphen_min(rhm),
        pat_len(p_l), pat_dot(p_d),
        good_wt(g_w), bad_wt(b_w), thresh(t),
        patterns(pat),
        good_count(0), bad_count(0), miss_count(0),
        candidates(patterns.get_max_in_alph(), 0, 0),
        word_input(translate, i_d_f_n) {
        hyf_min = left_hyphen_min + 1;
        hyf_max = right_hyphen_min + 1;
        hyf_len = hyf_min + hyf_max;
        dot_min = pat_dot;
        dot_max = pat_len - pat_dot;
        if (dot_min < hyf_min)
            dot_min = hyf_min;
        if (dot_max < hyf_max)
            dot_max = hyf_max;
        dot_len = dot_min + dot_max;
        if (hyph_level % 2 == 1) {
            good_dot = hyphenation_type::correct;
            bad_dot = hyphenation_type::none;
        } else {
            good_dot = hyphenation_type::wrong;
            bad_dot = hyphenation_type::past;
        }
    }

public:
    void hyphenate(THword& w) {
        TOutputs_of_a_pattern o;
        typename TOutputs_of_a_pattern::iterator i;
        patterns.competitive_pattern_output(w, o, hopeless_hyph_val);

        for (i = o.begin(); i != o.end(); ++i) {
            w.level[i->first] = i->second;
        }

        patterns.competitive_pattern_output(w, o, hopeless_hyph_val + 1);

        for (i = o.begin(); i != o.end(); ++i) {
            if (i->second >= hyph_level &&
                i->first >= dot_min && i->first <= w.size() - dot_max) {

                std::vector<Tin_alph> subw;
                for (Tindex j = i->first + 1 - pat_dot;
                     j <= i->first + pat_len - pat_dot; ++j) {
                    subw.push_back(w[j]);
                }
                TOutputs_of_a_pattern subwo;
                patterns.competitive_pattern_output(subw, subwo,
                                                    hopeless_hyph_val + 1);
                typename TOutputs_of_a_pattern::iterator val_on_pat_dot =
                        subwo.find(pat_dot);

                if (val_on_pat_dot != subwo.end())
                    if (val_on_pat_dot->second >= hyph_level)
                        w.is_knocked_out[i->first] = true;
            }
        }
    }

public:
    void change_dots(THword& w) {
        for (Tindex i = w.size() - hyf_max; i >= hyf_min; --i) {
            if (w.level[i] % 2 == 1) {
                if (w.type[i] == hyphenation_type::none)
                    w.type[i] = hyphenation_type::wrong;
                else if (w.type[i] == hyphenation_type::correct)
                    w.type[i] = hyphenation_type::past;
            }

            if (w.type[i] == hyphenation_type::past)
                good_count += w.weight[i];
            else if (w.type[i] == hyphenation_type::wrong)
                bad_count += w.weight[i];
            else if (w.type[i] == hyphenation_type::correct)
                miss_count += w.weight[i];
        }
    }

protected:
    void do_word(THword& w) {
        for (Tindex dpos = w.size() - dot_max; dpos >= dot_min; --dpos) {

            if (w.is_knocked_out[dpos])
                continue;
            if ((w.type[dpos] != good_dot) && (w.type[dpos] != bad_dot))
                continue;

            Tindex spos = dpos - pat_dot;
            Tindex fpos = spos + pat_len;
            ++spos;

            std::vector<Tin_alph> subw;
            for (Tindex i = spos; i <= fpos; ++i)
                subw.push_back(w[i]);

            if (w.type[dpos] == good_dot) {
                candidates.increment_counts(subw, w.weight[dpos], 0);
            } else {
                candidates.increment_counts(subw, 0, w.weight[dpos]);
            }
        }
    }

public:
    void print_pass_statistics() {
        std::cout << std::endl;
        std::cout << good_count << " good " << bad_count << " bad " << miss_count << " missed" << std::endl;
        if (good_count + miss_count > 0) {
            std::cout << 100.0 * good_count / float(good_count + miss_count) << " % ";
            std::cout << 100.0 * bad_count / float(good_count + miss_count) << " % ";
            std::cout << 100.0 * miss_count / float(good_count + miss_count) << " % " << std::endl;
        }
    }

protected:
    void do_dictionary() {
        THword w;
        while (word_input.get(w)) {
            if (w.size() >= hyf_len) {
                hyphenate(w);
                change_dots(w);
            }
            if (w.size() >= dot_len)
                do_word(w);
        }

#ifdef DEBUG

TOutputs_of_a_pattern o;
vector<Tin_alph> word;
patterns.init_walk_through();
std::cout<<"Patterns in the pattern manipulator:"<<std::endl;
while(patterns.get_next_pattern(word,o)){
std::cout<<"Word ";
for(vector<Tin_alph> ::iterator i= word.begin();i!=word.end();
i++)std::cout<<*i<<" ";
std::cout<<"... has ";
for(typename TOutputs_of_a_pattern::const_iterator i= o.begin();
i!=o.end();i++)
std::cout<<"("<<i->first<<","<<i->second<<") ";
std::cout<<std::endl;
}
vector<Tin_alph> vect;
Tcount_type a;Tcount_type b;
candidates.init_walk_through();
std::cout<<"Candidates:"<<std::endl;
while(candidates.get_next_pattern(vect,a,b)){
for(vector<Tin_alph> ::iterator i= vect.begin();i!=vect.end();i++)
std::cout<<*i<<" ";
std::cout<<"with g,b: "<<a<<","<<b<<std::endl;
}

#endif

        print_pass_statistics();
        std::cout << "Count data structure statistics:" << std::endl;
        candidates.print_statistics();
    }

protected:
    bool collect_candidates(Tcount_type& level_pattern_count) {
        std::cout << "Collecting candidates" << std::endl;

        bool more_to_come = false;
        std::vector<Tin_alph> w;
        Tcount_type g, b;
        Tcount_type good_pat_count = 0;
        Tcount_type bad_pat_count = 0;

        candidates.init_walk_through();
        while (candidates.get_next_pattern(w, g, b)) {
            if (good_wt * g < thresh) {
                patterns.insert_pattern(w, pat_dot, hopeless_hyph_val);
                ++bad_pat_count;
            } else {

                if (good_wt * g >= thresh + bad_wt * b) {
                    patterns.insert_pattern(w, pat_dot, hyph_level, 1);

                    ++good_pat_count;
                    good_count += g;
                    bad_count += b;
                } else
                    more_to_come = true;
            }
        }

        std::cout << good_pat_count << " good and " << bad_pat_count << " bad patterns added";
        if (more_to_come == true)
            std::cout << " (more to come)";
        std::cout << std::endl;
        std::cout << "finding " << good_count << " good and " << bad_count << " bad hyphens" << std::endl;
        if (good_pat_count > 0) {
            std::cout << "efficiency = "
                    << float(good_count) / (float(good_pat_count) + float(bad_count) /
                                            (float(thresh) / float(good_wt)))
                    << std::endl;
        }
        std::cout << "Pattern data structure statistics:" << std::endl;
        patterns.print_statistics();

        level_pattern_count += good_pat_count;
        return more_to_come;
    }

public:
    bool do_all(Tcount_type& level_pattern_count) {
        std::cout << std::endl << "Generating a pass with pat_len = " << pat_len
                << ", pat_dot = " << pat_dot << std::endl;

        do_dictionary();
        return collect_candidates(level_pattern_count);
    }

};

template <class Tindex, class Tin_alph, class Tval_type, class Twt_type,
          class Tcount_type, class THword, class TTranslate,
          class TCandidate_count_structure,
          class TCompetitive_multi_out_pat_manip,
          class TWord_input_file, class TPass>
class level {

protected:
    TTranslate& translate;
    const char* word_input_file_name;
    const Tval_type hyph_level;
    const Tval_type hopeless_hyph_val;
    const Tindex left_hyphen_min, right_hyphen_min;
    TCompetitive_multi_out_pat_manip& patterns;
    Tcount_type level_pattern_count;
    Tindex pat_start, pat_finish;
    Tcount_type good_wt, bad_wt, thresh;

public:
    level(TTranslate& tra, const char* i_d_f_n,
          const Tval_type& l, const Tval_type& h,
          const Tindex& lhm, const Tindex& rhm,
          TCompetitive_multi_out_pat_manip& p):
        translate(tra), word_input_file_name(i_d_f_n),
        hyph_level(l), hopeless_hyph_val(h),
        left_hyphen_min(lhm), right_hyphen_min(rhm),
        patterns(p),
        level_pattern_count(0) {
        do {
            std::cout << "pat_start, pat_finish: ";
            std::cin >> pat_start;
            std::cin >> pat_finish;
            if (pat_start < 1 || pat_start > pat_finish) {
                std::cout << "Specify two integers satisfying 1<=pat_start<=pat_finish ";
                pat_start = 0;
            }
        } while (pat_start < 1);
        do {
            std::cout << "good weight, bad weight, threshold: ";
            std::cin >> good_wt;
            std::cin >> bad_wt;
            std::cin >> thresh;
            if (good_wt < 1 || bad_wt < 1 || thresh < 1) {
                std::cout << "Specify three integers: good weight, bad weight, threshold>=1 ";
                good_wt = 0;
            }
        } while (good_wt < 1);
    }

public:
    void do_all() {
        std::cout << std::endl << std::endl << "Generating level " << hyph_level << std::endl;
        growing_array<char> more_this_level(true);
        for (Tindex pat_len = pat_start; pat_len <= pat_finish; ++pat_len) {

            Tindex pat_dot = pat_len / 2;
            Tindex dot1 = pat_dot * 2;
            do {
                pat_dot = dot1 - pat_dot;
                dot1 = pat_len * 2 - dot1 - 1;
                if (more_this_level[pat_dot]) {
                    TPass pass(translate, word_input_file_name,
                               hyph_level, hopeless_hyph_val,
                               left_hyphen_min, right_hyphen_min,
                               pat_len, pat_dot, good_wt,
                               bad_wt, thresh, patterns);
                    more_this_level[pat_dot] = pass.do_all(level_pattern_count);
                }
            } while (pat_dot != pat_len);
            for (Tindex k = more_this_level.size(); k >= 1; --k)
                if (more_this_level[k - 1] != true)
                    more_this_level[k] = false;
        }

        Tindex old_p_c = patterns.get_pat_count();
        patterns.delete_values(hopeless_hyph_val);
        std::cout << old_p_c - patterns.get_pat_count()
                << " bad patterns deleted" << std::endl;
        patterns.delete_hanging();

        std::cout << "total of " << level_pattern_count << " patterns at level "
                << hyph_level << std::endl;
    }

};

template <class Tindex, class Tin_alph, class Tval_type, class Twt_type,
          class Tcount_type, class THword, class TTranslate,
          class TCandidate_count_structure,
          class TCompetitive_multi_out_pat_manip,
          class TOutputs_of_a_pattern,
          class TWord_input_file, class TWord_output_file,
          class TPattern_input_file, class TPattern_output_file,
          class TPass, class TLevel>
class generator {

protected:
    TTranslate translate;
    const char* name;
    const char* word_input_file_name;
    const char* pattern_input_file_name;
    const char* pattern_output_file_name;
    TCompetitive_multi_out_pat_manip patterns;
    Tval_type hyph_start, hyph_finish;
    Tindex left_hyphen_min, right_hyphen_min;

public:
    generator(const char* dic, const char* pat,
              const char* out, const char* tra):
        translate(tra), word_input_file_name(dic),
        pattern_input_file_name(pat),
        pattern_output_file_name(out),
        patterns(translate.get_max_in_alph()),
        left_hyphen_min(translate.get_left_hyphen_min()),
        right_hyphen_min(translate.get_right_hyphen_min()) {
        do {
            std::cout << "hyph_start, hyph_finish: ";
            std::cin >> hyph_start;
            std::cin >> hyph_finish;
            if ((hyph_start < 1) || (hyph_finish < 1)) {
                hyph_start = 0;
                std::cout << "Specify two integers satisfying 1<=hyph_start, hyph_finish "
                        << std::endl;
            }
        } while (hyph_start < 1);
    }

    void read_patterns() {
        std::vector<Tin_alph> v;
        TOutputs_of_a_pattern o;

        TPattern_input_file file(translate, pattern_input_file_name);

        while (file.get(v, o)) {
            if (v.size() > 0) {
                for (typename TOutputs_of_a_pattern::iterator i = o.begin();
                     i != o.end(); ++i) {
                    if (i->second >= hyph_start) {
                        throw exception("! The patterns to be read in contain "
                            "hyphenation value bigger than hyph_start.");
                    }
                    patterns.insert_pattern(v, i->first, i->second);
                }
            }
        }
    }

    void output_patterns() {
        TPattern_output_file file(translate, pattern_output_file_name);

        std::vector<Tin_alph> v;
        TOutputs_of_a_pattern o;
        patterns.init_walk_through();
        while (patterns.get_next_pattern(v, o)) {
            file.put(v, o);
        }
    }

public:
    void hyphenate_word_list() {
        std::string s;
        std::cout << "hyphenate word list <y/n>? ";
        std::cin >> s;
        if (!(s == "y" || s == "Y"))
            return;

        Tval_type level_value = hyph_finish;
        if (hyph_start > hyph_finish)
            level_value = hyph_start;
        Tval_type fake_level_value = 2 * ((level_value / 2) + 1);

        char file_name[100];
        sprintf(file_name, "pattmp.%d", level_value);
        std::cout << "Writing file " << file_name << std::endl;

        THword w;
        TWord_output_file o_f(translate, file_name);
        TWord_input_file i_f(translate, word_input_file_name);
        TPass pass(translate, word_input_file_name,
                   level_value, fake_level_value,
                   left_hyphen_min, right_hyphen_min,
                   1, 1, 1, 1, 1, patterns);
        while (i_f.get(w)) {
            if (w.size() > 2) {
                pass.hyphenate(w);
                pass.change_dots(w);
            }
            o_f.put(w);
        }
        pass.print_pass_statistics();
    }

public:
    void do_all() {
        read_patterns();
        std::cout << patterns.get_pat_count() << " pattern lines read in" << std::endl;

        Tval_type hopeless_fake_number = 2 * ((hyph_finish / 2) + 1);

        for (Tval_type l = hyph_start; l <= hyph_finish; ++l) {
            TLevel level(translate, word_input_file_name,
                         l, hopeless_fake_number, left_hyphen_min,
                         right_hyphen_min, patterns);
            level.do_all();
        }
        output_patterns();
        hyphenate_word_list();
        std::cout << std::endl;
    }

};

} // namespace ptl
