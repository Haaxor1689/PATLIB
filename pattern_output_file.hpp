#pragma once

#include <fstream>
#include <vector>

namespace ptl {
    
template <class Tindex, class Tin_alph, class Tval_type, class TTranslate, class TOutputs_of_a_pattern>
class pattern_output_file {

    TTranslate& translate;
    const std::string file_name;
    std::basic_ofstream<unsigned char> file;

public:
    pattern_output_file(TTranslate& t, const char* fn) : translate(t), file_name(fn), file(file_name) {}

    void put(const std::vector<Tin_alph>& v, const TOutputs_of_a_pattern& o) {
        typename TOutputs_of_a_pattern::const_iterator oi;
        std::basic_string<unsigned char> s;

        Tindex pos = 0;
        for (const auto& vi : v) {
            oi = o.find(pos);
            if (oi != o.end()) {
                translate.get_xdig(oi->second, s);
            }

            ++pos;
            translate.get_xext(vi, s);
        }

        oi = o.find(pos);
        if (oi != o.end()) {
            translate.get_xdig(oi->second, s);
        }

        file << s << std::endl;
    }

};

} // namespace ptl