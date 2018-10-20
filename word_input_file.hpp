#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace ptl {
    
template <class THword, class TTranslate, class Tnum_type>
class word_input_file {

    const bool utf_8;

    TTranslate& translate;
    const std::string file_name;
    std::basic_ifstream<unsigned char> file;

    unsigned lineno = 0;

    typedef typename TTranslate::classified_symbol Tclassified_symbol;

    Tnum_type global_word_wt = 1;

public:
    word_input_file(TTranslate& t, const char* fn, bool utf_8) : utf_8(utf_8), translate(t), file_name(fn), file(file_name) {}

    bool get(THword& hw) {
        hw.clear();
        std::basic_string<unsigned char> s;

        if (!getline(file, s)) {
            return false;
        }

        ++lineno;

        s.push_back(' ');
        handle_line(s, hw);
        return true;
    }

private:
    void handle_line(const std::basic_string<unsigned char>& s, THword& hw) {
        hw.push_back(translate.get_edge_of_word());
        hw.weight[hw.size()] = global_word_wt;

        Tclassified_symbol i_class;
        auto i = s.begin();
        std::vector<unsigned char> seq;
        Tnum_type num;

        do {
            if (utf_8 && (*i & 0x80)) {
                {
                    unsigned char first_i = *i;
                    seq.clear();
                    while ((first_i & 0x80) && (*i & 0x80)) {

                        seq.push_back(*i);
                        ++i;
                        first_i = first_i << 1;
                    }
                    translate.classify(seq, i_class);
                    if (i_class.first == char_class::letter) {
                        hw.push_back(i_class.second);
                        hw.weight[hw.size()] = global_word_wt;
                    } else {
                        std::cerr << "! Error in " << file_name << " line " << lineno << ": " << "Multibyte sequence is invalid" << std::endl;
                        throw exception("");
                    }
                }
            } else {
                translate.classify(*i, i_class);
                switch (i_class.first) {
                case char_class::space:
                    goto done;
                case char_class::digit:
                    if (i == s.begin()) {
                        num = 0;
                        while (i_class.first == char_class::digit) {
                            num = 10 * num + i_class.second;
                            ++i;
                            translate.classify(*i, i_class);
                        }
                        hw.weight[hw.size()] = num;
                        global_word_wt = num;
                    } else {
                        num = 0;
                        while (i_class.first == char_class::digit) {
                            num = 10 * num + i_class.second;
                            ++i;
                            translate.classify(*i, i_class);
                        }
                        hw.weight[hw.size()] = num;
                    }
                    break;
                case char_class::hyf:
                    if (i_class.second == THword::correct || i_class.second == THword::past)
                        hw.type[hw.size()] = THword::correct;
                    ++i;
                    break;
                case char_class::letter:
                    hw.push_back(i_class.second);
                    hw.weight[hw.size()] = global_word_wt;
                    ++i;
                    break;
                case char_class::escape:

                    seq.clear();
                    seq.push_back(*i);

                    ++i;
                    translate.classify(*i, i_class);
                    while (i_class.first == char_class::letter ||
                           i_class.first == char_class::invalid) {
                        seq.push_back(*i);
                        ++i;
                        translate.classify(*i, i_class);
                    }

                    while (i_class.first == char_class::space && i != s.end()) {
                        ++i;
                        translate.classify(*i, i_class);
                    }
                    translate.classify(seq, i_class);

                    if (i_class.first == char_class::letter) {
                        hw.push_back(i_class.second);
                        hw.weight[hw.size()] = global_word_wt;
                    } else {
                        std::cerr << "! Error in " << file_name << " line " << lineno << ": "
                                << "Escape sequence is invalid" << std::endl;
                        std::cerr << "(Are you using correct encoding--the -u8 switch?)" << std::endl;
                        throw exception("");
                    }

                    break;
                default:
                    std::cerr << "! Error in " << file_name << " line " << lineno << ": "
                            << "Invalid character in input data" << std::endl;
                    throw exception("");
                }
            }
        } while (i != s.end());
    done:
        hw.push_back(translate.get_edge_of_word());
        hw.weight[hw.size()] = global_word_wt;
        hw.weight[0] = global_word_wt;
    }

};

} // namespace ptl
