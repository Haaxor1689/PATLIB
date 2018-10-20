#pragma once

#include <iostream>

#include "growing_array.hpp"
#include "pass.hpp"

namespace ptl {

class level {

    using Tindex = std::size_t;
    using Tin_alph = unsigned;
    using Tval_type = unsigned;
    using Twt_type = unsigned;
    using Tcount_type = unsigned;
    using THword = hyphenated_word;
    using TTranslate = translate;
    using TCandidate_count_structure = candidate_count_trie;
    using TCompetitive_multi_out_pat_manip = competitive_multi_out_pat_manip;
    using TWord_input_file = word_input_file;
    using TPass = pass;

    TTranslate& translate;
    const char* word_input_file_name;
    const Tval_type hyph_level;
    const Tval_type hopeless_hyph_val;
    const Tindex left_hyphen_min;
    const Tindex right_hyphen_min;
    TCompetitive_multi_out_pat_manip& patterns;
    Tcount_type level_pattern_count;
    Tindex pat_start;
    Tindex pat_finish;
    Tcount_type good_wt;
    Tcount_type bad_wt;
    Tcount_type thresh;

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
        std::cout << old_p_c - patterns.get_pat_count() << " bad patterns deleted" << std::endl;
        patterns.delete_hanging();

        std::cout << "total of " << level_pattern_count << " patterns at level " << hyph_level << std::endl;
    }

};
    
} // namespace ptl
