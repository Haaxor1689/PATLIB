#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "typedefs.hpp"
#include "ptl_mopm.hpp"

namespace ptl {
    
class pattern_input_file {

    const bool utf_8;

    translate& _translate;
    const std::string file_name;
    std::basic_ifstream<unsigned char> file;

    unsigned lineno = 0;

public:
    pattern_input_file(translate& t, const std::string& fn, bool utf_8) : utf_8(utf_8), _translate(t), file_name(fn), file(file_name) {}

    bool get(std::vector<Tin_alph>& v, outputs_of_a_pattern& o) {
        v.clear();
        o.clear();
        std::basic_string<unsigned char> s;

        if (!std::getline(file, s)) {
            return false;
        }

        lineno++;
        s.push_back(' ');
        handle_line(s, v, o);
        return true;
    }

private:
    void handle_line(const std::basic_string<unsigned char>& s, std::vector<Tin_alph>& v, outputs_of_a_pattern& o) {
        translate::classified_symbol i_class;
        auto i = s.begin();
        std::vector<unsigned char> seq;
        Tval_type num;

        std::size_t chars_read = 0;
        do {
            if (*i == '.') {
                v.push_back(_translate.get_edge_of_word());
                ++chars_read;
                ++i;
                continue;
            }

            if (utf_8 && *i > 127) {

                unsigned char first_i = *i;
                seq.clear();
                while (first_i & 0x80 && *i & 0x80) {
                    seq.push_back(*i);
                    ++i;
                    first_i = first_i << 1;
                }

                _translate.classify(seq, i_class);
                if (i_class.first == char_class::letter) {
                    v.push_back(i_class.second);
                    ++chars_read;
                } else {
                    std::cerr << "! Error in " << file_name << " line " << lineno << ": " << "Multibyte sequence is invalid" << std::endl;
                    throw exception("");
                }
            } else {
                _translate.classify(*i, i_class);
                switch (i_class.first) {
                case char_class::space:
                    goto done;
                case char_class::digit:

                    num = 0;
                    while (i_class.first == char_class::digit) {
                        num = 10 * num + i_class.second;
                        ++i;
                        _translate.classify(*i, i_class);
                    }

                    o.insert(std::make_pair(chars_read, num));

                    break;
                case char_class::letter:

                    v.push_back(i_class.second);
                    ++chars_read;
                    ++i;

                    break;
                case char_class::escape:

                    seq.clear();
                    seq.push_back(*i);

                    ++i;
                    _translate.classify(*i, i_class);
                    while (i_class.first == char_class::letter ||
                           i_class.first == char_class::invalid) {
                        seq.push_back(*i);
                        ++i;
                        _translate.classify(*i, i_class);
                    }

                    while (i_class.first == char_class::space && i != s.end()) {
                        ++i;
                        _translate.classify(*i, i_class);
                    }
                    _translate.classify(seq, i_class);

                    if (i_class.first == char_class::letter) {
                        v.push_back(i_class.second);
                        ++chars_read;
                    } else {
                        std::cerr << "! Error in " << file_name << " line " << lineno << ": "
                                  << "Escape sequence is invalid" << std::endl;
                        std::cerr << "(Are you using correct encoding--the -u8 switch?)" << std::endl;
                        throw exception("");
                    }

                    break;
                default:
                    std::cerr << "! Error in " << file_name << " line " << lineno << ": "
                              << "Invalid character in pattern data" << std::endl;
                    throw exception("");
                }
            }
        } while (i != s.end());
    done:;
    }

};

} // namespace ptl
