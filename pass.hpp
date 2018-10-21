#pragma once

#include <vector>
#include <iostream>

#include "typedefs.hpp"
#include "hyphenation_type.hpp"
#include "translate.hpp"
#include "ptl_mopm.hpp"
#include "word_input_file.hpp"
#include "candidate_count_trie.hpp"
#include "char_class.hpp"

namespace ptl {

class pass {

    translate& _translate;
    const Tval_type hyph_level;
    const Tval_type hopeless_hyph_val;
    const std::size_t left_hyphen_min;
    const std::size_t right_hyphen_min;
    const std::size_t pat_len;
    const std::size_t pat_dot;
    const Tcount_type good_wt;
    const Tcount_type bad_wt;
    const Tcount_type thresh;
    competitive_multi_out_pat_manip& patterns;

    Tcount_type good_count;
    Tcount_type bad_count;
    Tcount_type miss_count;
    candidate_count_trie candidates;

    word_input_file word_input;

    std::size_t hyf_min;
    std::size_t hyf_max;
    std::size_t hyf_len;
    std::size_t dot_min;
    std::size_t dot_max;
    std::size_t dot_len;
    hyphenation_type good_dot;
    hyphenation_type bad_dot;

public:
    pass(translate& tra, const std::string& i_d_f_n,
         const Tval_type& l, const Tval_type& h,
         const std::size_t& lhm, const std::size_t& rhm,
         const std::size_t& p_l, const std::size_t& p_d,
         const Twt_type& g_w, const Twt_type& b_w, const Twt_type& t,
         competitive_multi_out_pat_manip& pat, bool utf_8) :
        _translate(tra),
        hyph_level(l), hopeless_hyph_val(h),
        left_hyphen_min(lhm), right_hyphen_min(rhm),
        pat_len(p_l), pat_dot(p_d),
        good_wt(g_w), bad_wt(b_w), thresh(t),
        patterns(pat),
        good_count(0), bad_count(0), miss_count(0),
        candidates(patterns.get_max_in_alph(), Tcount_good(0), 0),
        word_input(_translate, i_d_f_n, utf_8) {
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

    void hyphenate(hyphenated_word& w) {
        outputs_of_a_pattern o;
        outputs_of_a_pattern::iterator i;
        patterns.competitive_pattern_output(w, o, hopeless_hyph_val);

        for (i = o.begin(); i != o.end(); ++i) {
            w.level[i->first] = i->second;
        }

        patterns.competitive_pattern_output(w, o, hopeless_hyph_val + 1);

        for (i = o.begin(); i != o.end(); ++i) {
            if (i->second >= hyph_level &&
                i->first >= dot_min && i->first <= w.size() - dot_max) {

                std::vector<Tin_alph> subw;
                for (std::size_t j = i->first + 1 - pat_dot;
                     j <= i->first + pat_len - pat_dot; ++j) {
                    subw.push_back(w[j]);
                }
                outputs_of_a_pattern subwo;
                patterns.competitive_pattern_output(subw, subwo,
                                                    hopeless_hyph_val + 1);
                outputs_of_a_pattern::iterator val_on_pat_dot =
                        subwo.find(pat_dot);

                if (val_on_pat_dot != subwo.end())
                    if (val_on_pat_dot->second >= hyph_level)
                        w.is_knocked_out[i->first] = true;
            }
        }
    }

    void change_dots(hyphenated_word& w) {
        for (std::size_t i = w.size() - hyf_max; i >= hyf_min; --i) {
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
        hyphenated_word w;
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

    void do_word(hyphenated_word& w) {
        for (std::size_t dpos = w.size() - dot_max; dpos >= dot_min; --dpos) {

            if (w.is_knocked_out[dpos])
                continue;
            if ((w.type[dpos] != good_dot) && (w.type[dpos] != bad_dot))
                continue;

            std::size_t spos = dpos - pat_dot;
            std::size_t fpos = spos + pat_len;
            ++spos;

            std::vector<Tin_alph> subw;
            for (std::size_t i = spos; i <= fpos; ++i)
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