#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <fstream>

#include "typedefs.hpp"
#include "hyphenation_type.hpp"
#include "io_word_manipulator.hpp"
#include "io_reverse_mapping.hpp"

namespace ptl {

enum class char_class {
    space,
    digit,
    hyf,
    letter,
    escape,
    invalid
};

class translate {

    bool utf_8;

    Tin_alph edge_of_word;

    std::size_t max_in_alph = 0;
    std::size_t left_hyphen_min;
    std::size_t right_hyphen_min;

    io_word_manipulator classified_symbols { 255, std::make_pair(char_class::invalid, 0), 0 };

    io_reverse_mapping<Tin_alph, Tin_alph> xdig;
    io_reverse_mapping<Tin_alph, hyphenation_type> xhyf;
    io_reverse_mapping<Tin_alph, Tin_alph> xext;

public:

    translate(const std::string& tra, bool utf_8) : utf_8(utf_8) {
        prepare_fixed_defaults();
        prepare_default_hyfs();
        read_translate(tra);
    }

    void classify(Tin_alph c, classified_symbol& o) {
        classified_symbols.word_output(c, o);
    }

    void classify(const std::vector<Tin_alph>& vc, classified_symbol& o) {
        classified_symbols.word_last_output(vc, o);
    }

    std::size_t get_max_in_alph() {
        return max_in_alph;
    }

    std::size_t get_right_hyphen_min() {
        return right_hyphen_min;
    }

    std::size_t get_left_hyphen_min() {
        return left_hyphen_min;
    }

    Tin_alph get_edge_of_word() {
        return edge_of_word;
    }

    void get_xdig(Tin_alph i, std::basic_string<Tin_alph>& e) {
        std::basic_string<Tin_alph> inv_rep;
        while (i > 0) {
            xdig.add_to_string((i % 10), inv_rep);
            i = Tin_alph(i / 10);
        }
        e.append(inv_rep.rbegin(), inv_rep.rend());
    }

    void get_xhyf(hyphenation_type i, std::basic_string<Tin_alph>& e) {
        xhyf.add_to_string(i, e);
    }

    void get_xext(const Tin_alph& i, std::basic_string<Tin_alph>& e) {
        xext.add_to_string(i, e);
    }

protected:

    Tin_alph get_next_internal_code() {
        return ++max_in_alph;
    }

    void prepare_fixed_defaults() {
        Tin_alph d;
        std::vector<Tin_alph> repres;

        for (d = 0; d <= 9; ++d) {
            classified_symbols.hard_insert_pattern({ d + '0' }, std::make_pair(char_class::digit, d));
            repres.clear();
            repres.push_back(d + '0');
            xdig.insert(d, repres);
        }

        classified_symbols.hard_insert_pattern({ ' ' }, std::make_pair(char_class::space, 0));
        classified_symbols.hard_insert_pattern({ 9 }, std::make_pair(char_class::space, 0));

        edge_of_word = get_next_internal_code();

        std::vector<Tin_alph> edge_of_word_printable;
        edge_of_word_printable.push_back('.');
        xext.insert(edge_of_word, edge_of_word_printable);
    }

    void prepare_default_hyfs() {
        std::vector<Tin_alph> repres;

        classified_symbols.hard_insert_pattern({ '.' }, std::make_pair(char_class::hyf, 1 /*hyphenation_type::wrong*/));
        repres.clear();
        repres.push_back('.');
        xhyf.insert(hyphenation_type::wrong, repres);
        classified_symbols.hard_insert_pattern({ '-' }, std::make_pair(char_class::hyf, 2 /*hyphenation_type::correct*/));
        repres.clear();
        repres.push_back('-');
        xhyf.insert(hyphenation_type::correct, repres);
        classified_symbols.hard_insert_pattern({ '*' }, std::make_pair(char_class::hyf, 3 /*hyphenation_type::past*/));
        repres.clear();
        repres.push_back('*');
        xhyf.insert(hyphenation_type::past, repres);
    }

    void prepare_default_alphabet() {
        std::vector<Tin_alph> repres;

        for (Tin_alph c = 'a'; c <= 'z'; c++) {
            Tin_alph internal = get_next_internal_code();
            classified_symbols.hard_insert_pattern({ c }, std::make_pair(char_class::letter, internal));
            classified_symbols.hard_insert_pattern({ c + 'A' - 'a' }, std::make_pair(char_class::letter, internal));
            repres.clear();
            repres.push_back(c);
            xext.insert(internal, repres);
        }
    }

    void handle_preamble_of_translate(const std::basic_string<Tin_alph>& s) {
        std::size_t n = 0;
        bool bad = false;
        classified_symbol cs;

        if (s.length() >= 4) {
            classify(s[0], cs);
            if (cs.first == char_class::space)
                n = 0;
            else {
                if (cs.first == char_class::digit)
                    n = cs.second;
                else
                    bad = true;
            }
            classify(s[1], cs);
            if (cs.first == char_class::digit)
                n = 10 * n + cs.second;
            else
                bad = true;

            if (n >= 1)
                left_hyphen_min = n;
            else
                bad = true;

            classify(s[2], cs);
            if (cs.first == char_class::space)
                n = 0;
            else {
                if (cs.first == char_class::digit)
                    n = cs.second;
                else
                    bad = true;
            }
            classify(s[3], cs);
            if (cs.first == char_class::digit)
                n = 10 * n + cs.second;
            else
                bad = true;

            if (n >= 1)
                right_hyphen_min = n;
            else
                bad = true;
        } else {
            bad = true;
        }

        if (bad) {
            std::size_t n1;
            std::size_t n2;
            std::cout << "! Values of left_hyphen_min and right_hyphen_min in translate are invalid." << std::endl;
            do {
                std::cout << "left_hyphen_min, right_hyphen_min: ";
                std::cin >> n1 >> n2;
                if (n1 >= 1 && n2 >= 1) {
                    left_hyphen_min = n1;
                    right_hyphen_min = n2;
                } else {
                    n1 = 0;
                    std::cout << "Specify 1<=left_hyphen_min, right_hyphen_min!" << std::endl;
                }
            } while (n1 <= 0);
        }

        for (std::size_t i = 1 /*hyphenation_type::wrong*/; i <= 3 /*hyphenation_type::past*/; ++i) {
            if (s.length() - 1 >= i + 3) {
                classify(s[i + 3], cs);
                if (utf_8 && s[i + 3] > 0x80) {
                    throw ptl::exception("! Error reading translate file, In the first line, "
                        "specifying hyf characters:\n"
                        "In UTF-8 mode 8-bit symbol is not allowed.");
                }
                if (cs.first == char_class::space)
                    continue;
                if (cs.first == char_class::invalid) {

                    std::vector<Tin_alph> v;
                    v.push_back(s[i + 3]);
                    xhyf.insert(static_cast<hyphenation_type>(i), v);
                    classified_symbols.hard_insert_pattern({ s[i + 3] }, std::make_pair(char_class::hyf, i));
                } else {
                    throw exception("! Error reading translate file. In the first line, specifying hyf characters:\n"
                                         "Specified symbol has been already assigned.");
                }
            }
        }
    }

    void handle_line_of_translate(std::basic_string<Tin_alph>& s, const unsigned& lineno) {
        if (s.length() == 0) {
            return;
        }

        bool primary_repres = true;
        std::vector<Tin_alph> letter_repres;
        Tin_alph internal;
        auto delimiter = *s.begin();

        s += delimiter + delimiter;

        auto i = s.begin();
        while (true) {
            ++i;
            if (*i == delimiter)
                break;

            letter_repres.clear();
            while (*i != delimiter) {
                letter_repres.push_back(*i);
                ++i;
            }
            if (primary_repres)
                internal = get_next_internal_code();

            classified_symbol cs;
            if (letter_repres.size() == 1) {
                classify(*letter_repres.begin(), cs);
                if (utf_8 && *letter_repres.begin() > 127) {
                    std::cout << "! Warning: Translate file, line " << lineno << ":" << std::endl;
                    std::cout << "There is single 8-bit ASCII character, it is probably an error ";
                    std::cout << "in UTF-8 mode" << std::endl;
                }
                if (cs.first == char_class::invalid) {
                    classified_symbols.hard_insert_pattern(letter_repres, std::make_pair(char_class::letter, internal));
                } else {
                    std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                    std::cerr << "Trying to redefine previously defined character" << std::endl;
                    throw ptl::exception("");
                }
            } else {
                classify(*letter_repres.begin(), cs);
                if (cs.first == char_class::invalid)
                    classified_symbols.hard_insert_pattern({ letter_repres[0] }, std::make_pair(char_class::escape, 0));
                classify(*letter_repres.begin(), cs);
                if (cs.first != char_class::escape) {
                    std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                    std::cerr << "The first symbol of multi-char or UTF-8 sequence has been ";
                    std::cerr << "used before";
                    std::cerr << std::endl << "as non-escape character" << std::endl;
                    throw ptl::exception("");
                }
                classify(letter_repres, cs);
                if (cs.first != char_class::invalid) {
                    std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                    std::cerr << "Trying to redefine previously defined character" << std::endl;
                    throw ptl::exception("");
                }

                if (utf_8) {
                    Tin_alph first = *letter_repres.begin();
                    unsigned expected_length = 0;
                    while (first & 0x80) {
                        expected_length++;
                        first = first << 1;
                    }
                    if (letter_repres.size() != expected_length) {
                        std::cout << "! Warning: Translate file, line " << lineno << ":" << std::endl;
                        std::cout << "UTF-8 sequence seems to be broken, it is probably an error." << std::endl;
                    }
                }

                classified_symbols.hard_insert_pattern(letter_repres, std::make_pair(char_class::letter, internal));
            }
            if (primary_repres)
                xext.insert(internal, letter_repres);
        
            primary_repres = false;
        }
    }

    void read_translate(const std::string& tra) {
        unsigned lineno = 1;
        std::basic_ifstream<Tin_alph> transl(tra);
        std::basic_string<Tin_alph> s;

        if (std::getline(transl, s)) {
            handle_preamble_of_translate(s);
            while (std::getline(transl, s))
                handle_line_of_translate(s, ++lineno);
        } else {
            std::cout << "Translate file does not exist or is empty. Defaults used." << std::endl;
            prepare_default_alphabet();
            left_hyphen_min = 2;
            right_hyphen_min = 3;
        }

        std::cout << "left_hyphen_min = " << left_hyphen_min << ", right_hyphen_min = " << right_hyphen_min << std::endl
                  << max_in_alph - edge_of_word << " letters" << std::endl;
    }

};
    
} // namespace ptl