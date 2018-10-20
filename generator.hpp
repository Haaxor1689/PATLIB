#pragma once

#include <iostream>
#include <vector>

#include "typedefs.hpp"
#include "exception.hpp"
#include "translate.hpp"
#include "hyphenated_word.hpp"
#include "pass.hpp"
#include "level.hpp"
#include "ptl_mopm.hpp"
#include "word_input_file.hpp"
#include "word_output_file.hpp"
#include "pattern_input_file.hpp"
#include "pattern_output_file.hpp"

namespace ptl {

class generator {

    translate _translate;
    const std::string name;
    const std::string word_input_file_name;
    const std::string pattern_input_file_name;
    const std::string pattern_output_file_name;
    competitive_multi_out_pat_manip patterns;
    Tval_type hyph_start;
    Tval_type hyph_finish;
    std::size_t left_hyphen_min;
    std::size_t right_hyphen_min;

public:
    generator(const std::string& dic, const std::string& pat, const std::string& out, const std::string& tra):
        _translate(tra), word_input_file_name(dic),
        pattern_input_file_name(pat),
        pattern_output_file_name(out),
        patterns(_translate.get_max_in_alph()),
        left_hyphen_min(_translate.get_left_hyphen_min()),
        right_hyphen_min(_translate.get_right_hyphen_min()) {
        do {
            std::cout << "hyph_start, hyph_finish: ";
            std::cin >> hyph_start;
            std::cin >> hyph_finish;
            if ((hyph_start < 1) || (hyph_finish < 1)) {
                hyph_start = 0;
                std::cout << "Specify two integers satisfying 1<=hyph_start, hyph_finish " << std::endl;
            }
        } while (hyph_start < 1);
    }

    void read_patterns() {
        std::vector<Tin_alph> v;
        outputs_of_a_pattern o;

        pattern_input_file file(_translate, pattern_input_file_name);

        while (file.get(v, o)) {
            if (v.size() > 0) {
                for (typename outputs_of_a_pattern::iterator i = o.begin();
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
        pattern_output_file file(_translate, pattern_output_file_name);

        std::vector<Tin_alph> v;
        outputs_of_a_pattern o;
        patterns.init_walk_through();
        while (patterns.get_next_pattern(v, o)) {
            file.put(v, o);
        }
    }

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
        sprintf_s(file_name, "pattmp.%d", level_value);
        std::cout << "Writing file " << file_name << std::endl;

        hyphenated_word w;
        word_output_file o_f(_translate, file_name);
        word_input_file i_f(_translate, word_input_file_name);
        pass pass(_translate, word_input_file_name,
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

    void do_all() {
        read_patterns();
        std::cout << patterns.get_pat_count() << " pattern lines read in" << std::endl;

        Tval_type hopeless_fake_number = 2 * ((hyph_finish / 2) + 1);

        for (Tval_type l = hyph_start; l <= hyph_finish; ++l) {
            level _level(_translate, word_input_file_name,
                         l, hopeless_fake_number, left_hyphen_min,
                         right_hyphen_min, patterns);
            _level.do_all();
        }
        output_patterns();
        hyphenate_word_list();
        std::cout << std::endl;
    }

};

} // namespace ptl
