#pragma once

#include <fstream>
#include <string>

#include "hyphenation_type.hpp"

namespace ptl {
    
class word_output_file {

    using Tindex = std::size_t;
    using THword = hyphenated_word;
    using TTranslate = translate;

    TTranslate& translate;
    const std::string file_name;
    std::basic_ofstream<unsigned char> file;

    unsigned last_global_word_wt = 1;
    unsigned global_word_wt = 0;

public:
    word_output_file(TTranslate& t, const char* fn) : translate(t), file_name(fn), file(file_name) {}

    void put(THword& hw) {
        std::basic_string<unsigned char> s;

        global_word_wt = hw.weight[0];
        if (last_global_word_wt != global_word_wt) {
            translate.get_xdig(hw.weight[0], s);
            last_global_word_wt = global_word_wt;
        }

        if (hw.type[1] != hyphenation_type::none) {
            translate.get_xhyf(hw.type[1], s);
        }

        for (Tindex dpos = 2; dpos <= hw.size() - 1; ++dpos) {
            translate.get_xext(hw[dpos], s);
            if (hw.type[dpos] != hyphenation_type::none)
                translate.get_xhyf(hw.type[dpos], s);
            if (hw.weight[dpos] != global_word_wt)
                translate.get_xdig(hw.weight[dpos], s);
        }

        file << s << std::endl;
    }

};

} // namespace ptl
