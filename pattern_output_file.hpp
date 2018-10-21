#pragma once

#include <fstream>
#include <vector>

namespace ptl {
    
class pattern_output_file {

    translate& _translate;
    const std::string file_name;
    std::basic_ofstream<Tin_alph> file;

public:
    pattern_output_file(translate& t, const std::string& fn) : _translate(t), file_name(fn), file(file_name) {}

    void put(const std::vector<Tin_alph>& v, const outputs_of_a_pattern& o) {
        outputs_of_a_pattern::const_iterator oi;
        std::basic_string<Tin_alph> s;

        std::size_t pos = 0;
        for (const auto& vi : v) {
            oi = o.find(pos);
            if (oi != o.end()) {
                _translate.get_xdig(oi->second, s);
            }

            ++pos;
            _translate.get_xext(vi, s);
        }

        oi = o.find(pos);
        if (oi != o.end()) {
            _translate.get_xdig(oi->second, s);
        }

        file << s << std::endl;
    }

};

} // namespace ptl
