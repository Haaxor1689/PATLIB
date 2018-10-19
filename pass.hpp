#pragma once

#include <vector>
#include <iostream>

#include "hyphenation_type.hpp"

namespace ptl {

template <class Tindex, class Tin_alph, class Tval_type, class Twt_type,
          class Tcount_type, class THword, class TTranslate,
          class TCandidate_count_structure,
          class TCompetitive_multi_out_pat_manip,
          class TOutputs_of_a_pattern,
          class TWord_input_file>
class pass {

    TTranslate& translate;
    const Tval_type hyph_level;
    const Tval_type hopeless_hyph_val;
    const Tindex left_hyphen_min;
    const Tindex right_hyphen_min;
    const Tindex pat_len;
    const Tindex pat_dot;
    const Tcount_type good_wt;
    const Tcount_type bad_wt;
    const Tcount_type thresh;
    TCompetitive_multi_out_pat_manip& patterns;

    Tcount_type good_count;
    Tcount_type bad_count;
    Tcount_type miss_count;
    TCandidate_count_structure candidates;

    TWord_input_file word_input;

    Tindex hyf_min;
    Tindex hyf_max;
    Tindex hyf_len;
    Tindex dot_min;
    Tindex dot_max;
    Tindex dot_len;
    hyphenation_type good_dot;
    hyphenation_type bad_dot;

public:
    pass(TTranslate& tra, const char* i_d_f_n,
         const Tval_type& l, const Tval_type& h,
         const Tindex& lhm, const Tindex& rhm,
         const Tindex& p_l, const Tindex& p_d,
         const Twt_type& g_w, const Twt_type& b_w, const Twt_type& t,
         TCompetitive_multi_out_pat_manip& pat) :
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

        if (dot_min < hyf_min) {
            dot_min = hyf_min;
        }

        if (dot_max < hyf_max) {
            dot_max = hyf_max;
        }

        dot_len = dot_min + dot_max;

        if (hyph_level % 2 == 1) {
            good_dot = hyphenation_type::correct;
            bad_dot = hyphenation_type::none;
        } else {
            good_dot = hyphenation_type::wrong;
            bad_dot = hyphenation_type::past;
        }
    }

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

    void print_pass_statistics() {
        std::cout << std::endl;
        std::cout << good_count << " good " << bad_count << " bad " << miss_count << " missed" << std::endl;
        if (good_count + miss_count > 0) {
            std::cout << 100.0 * good_count / float(good_count + miss_count) << " % ";
            std::cout << 100.0 * bad_count / float(good_count + miss_count) << " % ";
            std::cout << 100.0 * miss_count / float(good_count + miss_count) << " % " << std::endl;
        }
    }
    
    bool do_all(Tcount_type& level_pattern_count) {
        std::cout << std::endl << "Generating a pass with pat_len = " << pat_len
                << ", pat_dot = " << pat_dot << std::endl;

        do_dictionary();
        return collect_candidates(level_pattern_count);
    }

private:
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
        std::cout << "Patterns in the pattern manipulator:" << std::endl;
        while (patterns.get_next_pattern(word, o)) {
            std::cout << "Word ";
            for (vector<Tin_alph>::iterator i = word.begin(); i != word.end();
                 i++)
                std::cout << *i << " ";
            std::cout << "... has ";
            for (typename TOutputs_of_a_pattern::const_iterator i = o.begin();
                 i != o.end(); ++i)
                std::cout << "(" << i->first << "," << i->second << ") ";
            std::cout << std::endl;
        }
        vector<Tin_alph> vect;
        Tcount_type a;
        Tcount_type b;
        candidates.init_walk_through();
        std::cout << "Candidates:" << std::endl;
        while (candidates.get_next_pattern(vect, a, b)) {
            for (vector<Tin_alph>::iterator i = vect.begin(); i != vect.end(); i++)
                std::cout << *i << " ";
            std::cout << "with g,b: " << a << "," << b << std::endl;
        }
#endif

        print_pass_statistics();
        std::cout << "Count data structure statistics:" << std::endl;
        candidates.print_statistics();
    }

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

};

} // namespace ptl