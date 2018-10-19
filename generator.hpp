#pragma once

#include <iostream>

#include "exception.hpp"

namespace ptl {

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
