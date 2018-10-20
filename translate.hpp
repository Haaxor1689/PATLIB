#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <fstream>

#include "io_word_manipulator.hpp"
#include "io_reverse_mapping.hpp"

namespace ptl {

template <class Tindex, class Tnum_type, class THword>
class Translate {

    bool utf_8;

public:
    typedef enum {
        space_class,
        digit_class,
        hyf_class,
        letter_class,
        escape_class,

        invalid_class
    } Tcharacter_class;

    typedef std::pair<Tcharacter_class, Tnum_type> Tclassified_symbol;

protected:
    Tnum_type edge_of_word;

    Tindex max_in_alph;
    Tindex left_hyphen_min;
    Tindex right_hyphen_min;

    io_word_manipulator<Tindex, unsigned char, Tclassified_symbol> classified_symbols;

    io_reverse_mapping<unsigned char, Tnum_type> xdig;
    io_reverse_mapping<unsigned char, typename THword::hyphenation_type> xhyf;
    io_reverse_mapping<unsigned char, Tnum_type> xext;

    Tnum_type get_next_internal_code() {
        ++max_in_alph;
        return max_in_alph;
    }

public:
    void classify(const unsigned char& c, Tclassified_symbol& o) {
        classified_symbols.word_output(c, o);
    }

    void classify(const std::vector<unsigned char>& vc, Tclassified_symbol& o) {
        classified_symbols.word_last_output(vc, o);
    }

protected:
    void prepare_fixed_defaults() {
        Tnum_type d;
        std::vector<unsigned char> repres;

        for (d = 0; d <= 9; ++d) {
            classified_symbols.hard_insert_pattern((d + '0'),
                                                   make_pair(digit_class, d));
            repres.clear();
            repres.push_back(d + '0');
            xdig.insert(d, repres);
        }

        classified_symbols.hard_insert_pattern(' ', std::make_pair(space_class, 0));
        classified_symbols.hard_insert_pattern(9, std::make_pair(space_class, 0));

        edge_of_word = get_next_internal_code();
        std::vector<unsigned char> edge_of_word_printable;
        edge_of_word_printable.push_back('.');
        xext.insert(edge_of_word, edge_of_word_printable);
    }

    void prepare_default_hyfs() {
        std::vector<unsigned char> repres;

        classified_symbols.hard_insert_pattern('.', make_pair(hyf_class,
                                                              THword::wrong));
        repres.clear();
        repres.push_back('.');
        xhyf.insert(THword::wrong, repres);
        classified_symbols.hard_insert_pattern('-', make_pair(hyf_class,
                                                              THword::correct));
        repres.clear();
        repres.push_back('-');
        xhyf.insert(THword::correct, repres);
        classified_symbols.hard_insert_pattern('*', make_pair(hyf_class,
                                                              THword::past));
        repres.clear();
        repres.push_back('*');
        xhyf.insert(THword::past, repres);
    }

    void prepare_default_alphabet() {
        std::vector<unsigned char> repres;
        Tnum_type internal;

        for (unsigned char c = 'a'; c <= 'z'; c++) {
            internal = get_next_internal_code();
            classified_symbols.hard_insert_pattern(c, make_pair(letter_class,
                                                                internal));
            classified_symbols.hard_insert_pattern(c + 'A' - 'a',
                                                   make_pair(letter_class, internal));
            repres.clear();
            repres.push_back(c);
            xext.insert(internal, repres);
        }
    }

    void handle_preamble_of_translate(const std::basic_string<unsigned char>& s) {
        Tindex n = 0;
        bool bad = false;
        Tclassified_symbol cs;

        if (s.length() >= 4) {
            classify(s[0], cs);
            if (cs.first == space_class)
                n = 0;
            else {
                if (cs.first == digit_class)
                    n = cs.second;
                else
                    bad = true;
            }
            classify(s[1], cs);
            if (cs.first == digit_class)
                n = 10 * n + cs.second;
            else
                bad = true;

            if (n >= 1)
                left_hyphen_min = n;
            else
                bad = true;

            classify(s[2], cs);
            if (cs.first == space_class)
                n = 0;
            else {
                if (cs.first == digit_class)
                    n = cs.second;
                else
                    bad = true;
            }
            classify(s[3], cs);
            if (cs.first == digit_class)
                n = 10 * n + cs.second;
            else
                bad = true;

            if (n >= 1)
                right_hyphen_min = n;
            else
                bad = true;
        } else
            bad = true;

        if (bad) {
            bad = false;
            Tindex n1;
            Tindex n2;
            std::cout << "! Values of left_hyphen_min and right_hyphen_min in translate";
            std::cout << " are invalid." << std::endl;
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
            } while (!n1 > 0);
        }

        for (Tindex i = THword::wrong; i <= THword::past; ++i) {

            if (s.length() - 1 >= i + 3) {
                classify(s[i + 3], cs);
                if (utf_8 && s[i + 3] > 0x80) {
                    throw ptl::exception("! Error reading translate file, In the first line, "
                        "specifying hyf characters:\n"
                        "In UTF-8 mode 8-bit symbol is not allowed.");
                }
                if (cs.first == space_class)
                    continue;
                if (cs.first == invalid_class) {

                    std::vector<unsigned char> v;
                    v.push_back(s[i + 3]);
                    xhyf.insert(static_cast<hyphenation_type>(i), v);
                    classified_symbols.hard_insert_pattern(s[i + 3],
                                                           make_pair(hyf_class, i));
                } else {
                    throw ptl::exception("! Error reading translate file. In the first line, "
                        "specifying hyf characters:\n"
                        "Specified symbol has been already assigned.");
                }
            }
        }
    }

    void handle_line_of_translate(std::basic_string<unsigned char>& s, const unsigned& lineno) {
        if (s.length() == 0)
            return;

        bool primary_repres = true;
        std::vector<unsigned char> letter_repres;
        Tnum_type internal;
        unsigned char delimiter = *s.begin();

        s = s + delimiter + delimiter;

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

            {
                Tclassified_symbol cs;
                if (letter_repres.size() == 1) {
                    classify(*letter_repres.begin(), cs);
                    if (utf_8 && *letter_repres.begin() > 127) {
                        std::cout << "! Warning: Translate file, line " << lineno << ":" << std::endl;
                        std::cout << "There is single 8-bit ASCII character, it is probably an error ";
                        std::cout << "in UTF-8 mode" << std::endl;
                    }
                    if (cs.first == invalid_class) {
                        classified_symbols.hard_insert_pattern(letter_repres,
                                                               make_pair(letter_class, internal));
                    } else {
                        std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                        std::cerr << "Trying to redefine previously defined character" << std::endl;
                        throw ptl::exception("");
                    }
                } else {
                    classify(*letter_repres.begin(), cs);
                    if (cs.first == invalid_class)
                        classified_symbols.hard_insert_pattern(*letter_repres.begin(), std::make_pair(escape_class, 0));
                    classify(*letter_repres.begin(), cs);
                    if (cs.first != escape_class) {
                        std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                        std::cerr << "The first symbol of multi-char or UTF-8 sequence has been ";
                        std::cerr << "used before";
                        std::cerr << std::endl << "as non-escape character" << std::endl;
                        throw ptl::exception("");
                    }
                    classify(letter_repres, cs);
                    if (cs.first != invalid_class) {
                        std::cerr << "! Error: Translate file, line " << lineno << ":" << std::endl;
                        std::cerr << "Trying to redefine previously defined character" << std::endl;
                        throw ptl::exception("");
                    }

                    if (utf_8) {
                        unsigned char first = *letter_repres.begin();
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

                    classified_symbols.hard_insert_pattern(letter_repres,
                                                           make_pair(letter_class, internal));
                }
                if (primary_repres)
                    xext.insert(internal, letter_repres);
            }
            primary_repres = false;
        }
    }

    void read_translate(const char* tra) {
        unsigned lineno = 1;
        std::basic_ifstream<unsigned char> transl(tra);
        std::basic_string<unsigned char> s;

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

        std::cout << "left_hyphen_min = " << left_hyphen_min << ", right_hyphen_min = "
                << right_hyphen_min << std::endl
                << max_in_alph - edge_of_word << " letters" << std::endl;
    }

public:
    Translate(const char* tra):
        max_in_alph(0),
        classified_symbols(255, std::make_pair(invalid_class, 0)) {
        prepare_fixed_defaults();
        prepare_default_hyfs();
        read_translate(tra);
    }

    Tindex get_max_in_alph() {
        return max_in_alph;
    }

    Tindex get_right_hyphen_min() {
        return right_hyphen_min;
    }

    Tindex get_left_hyphen_min() {
        return left_hyphen_min;
    }

    unsigned char get_edge_of_word() {
        return edge_of_word;
    }

    void get_xdig(Tnum_type i, std::basic_string<unsigned char>& e) {
        std::basic_string<unsigned char> inv_rep;
        while (i > 0) {
            xdig.add_to_string((i % 10), inv_rep);
            i = Tnum_type(i / 10);
        }
        e.append(inv_rep.rbegin(), inv_rep.rend());
    }

    void get_xhyf(const typename THword::hyphenation_type& i, std::basic_string<unsigned char>& e) {
        xhyf.add_to_string(i, e);
    }

    void get_xext(const Tnum_type& i, std::basic_string<unsigned char>& e) {
        xext.add_to_string(i, e);
    }
};
    
} // namespace ptl