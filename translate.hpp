#pragma once

namespace ptl {

template <class Tindex, class Tnum_type, class THword>
class Translate {

public:
    typedef enum {
        space_class,
        digit_class,
        hyf_class,
        letter_class,
        escape_class,

        invalid_class
    } Tcharacter_class;

    typedef unsigned char Tfile_unit;

    typedef pair<Tcharacter_class, Tnum_type> Tclassified_symbol;

protected:
    Tnum_type edge_of_word;

    Tindex max_in_alph;
    Tindex left_hyphen_min;
    Tindex right_hyphen_min;

    io_word_manipulator<Tindex, Tfile_unit, Tclassified_symbol> classified_symbols;

    io_reverse_mapping<Tfile_unit, Tnum_type> xdig;
    io_reverse_mapping<Tfile_unit, typename THword::hyphenation_type> xhyf;
    io_reverse_mapping<Tfile_unit, Tnum_type> xext;

    Tnum_type get_next_internal_code(void) {
        ++max_in_alph;
        return max_in_alph;
    }

public:
    void classify(const Tfile_unit& c, Tclassified_symbol& o) {
        classified_symbols.word_output(c, o);
    }

    void classify(const vector<Tfile_unit>& vc, Tclassified_symbol& o) {
        classified_symbols.word_last_output(vc, o);
    }

protected:
    void prepare_fixed_defaults(void) {
        Tnum_type d;
        vector<Tfile_unit> repres;

        for (d = 0; d <= 9; ++d) {
            classified_symbols.hard_insert_pattern((d + '0'),
                                                   make_pair(digit_class, d));
            repres.clear();
            repres.push_back(d + '0');
            xdig.insert(d, repres);
        }

        classified_symbols.hard_insert_pattern(' ', make_pair(space_class, 0));
        classified_symbols.hard_insert_pattern(9, make_pair(space_class, 0));

        edge_of_word = get_next_internal_code();
        vector<Tfile_unit> edge_of_word_printable;
        edge_of_word_printable.push_back('.');
        xext.insert(edge_of_word, edge_of_word_printable);
    }

    void prepare_default_hyfs(void) {
        vector<Tfile_unit> repres;

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

    void prepare_default_alphabet(void) {
        vector<Tfile_unit> repres;
        Tnum_type internal;

        for (Tfile_unit c = 'a'; c <= 'z'; c++) {
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

    void handle_preamble_of_translate(const basic_string<Tfile_unit>& s) {
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
            cout << "! Values of left_hyphen_min and right_hyphen_min in translate";
            cout << " are invalid." << endl;
            do {
                cout << "left_hyphen_min, right_hyphen_min: ";
                cin >> n1 >> n2;
                if (n1 >= 1 && n2 >= 1) {
                    left_hyphen_min = n1;
                    right_hyphen_min = n2;
                } else {
                    n1 = 0;
                    cout << "Specify 1<=left_hyphen_min, right_hyphen_min!" << endl;
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

                    vector<Tfile_unit> v;
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

    void handle_line_of_translate(basic_string<Tfile_unit>& s,
                                  const unsigned& lineno) {
        if (s.length() == 0)
            return;

        bool primary_repres = true;
        vector<Tfile_unit> letter_repres;
        Tnum_type internal;
        Tfile_unit delimiter = *s.begin();

        s = s + delimiter + delimiter;

        basic_string<Tfile_unit>::const_iterator i = s.begin();

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
                        cout << "! Warning: Translate file, line " << lineno << ":" << endl;
                        cout << "There is single 8-bit ASCII character, it is probably an error ";
                        cout << "in UTF-8 mode" << endl;
                    }
                    if (cs.first == invalid_class) {
                        classified_symbols.hard_insert_pattern(letter_repres,
                                                               make_pair(letter_class, internal));
                    } else {
                        cerr << "! Error: Translate file, line " << lineno << ":" << endl;
                        cerr << "Trying to redefine previously defined character" << endl;
                        throw ptl::exception("");
                    }
                } else {
                    classify(*letter_repres.begin(), cs);
                    if (cs.first == invalid_class)
                        classified_symbols.hard_insert_pattern(*letter_repres.begin(),
                                                               make_pair(escape_class, 0));
                    classify(*letter_repres.begin(), cs);
                    if (cs.first != escape_class) {
                        cerr << "! Error: Translate file, line " << lineno << ":" << endl;
                        cerr << "The first symbol of multi-char or UTF-8 sequence has been ";
                        cerr << "used before";
                        cerr << endl << "as non-escape character" << endl;
                        throw ptl::exception("");
                    }
                    classify(letter_repres, cs);
                    if (cs.first != invalid_class) {
                        cerr << "! Error: Translate file, line " << lineno << ":" << endl;
                        cerr << "Trying to redefine previously defined character" << endl;
                        throw ptl::exception("");
                    }

                    if (utf_8) {
                        Tfile_unit first = *letter_repres.begin();
                        unsigned expected_length = 0;
                        while (first & 0x80) {
                            expected_length++;
                            first = first << 1;
                        }
                        if (letter_repres.size() != expected_length) {
                            cout << "! Warning: Translate file, line " << lineno << ":" << endl;
                            cout << "UTF-8 sequence seems to be broken, it is probably an error." << endl;
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
        ifstream transl(tra);
        basic_string<Tfile_unit> s;

        if (getline(transl, s)) {
            handle_preamble_of_translate(s);
            while (getline(transl, s))
                handle_line_of_translate(s, ++lineno);
        } else {
            cout << "Translate file does not exist or is empty. Defaults used." << endl;
            prepare_default_alphabet();
            left_hyphen_min = 2;
            right_hyphen_min = 3;
        }

        cout << "left_hyphen_min = " << left_hyphen_min << ", right_hyphen_min = "
                << right_hyphen_min << endl
                << max_in_alph - edge_of_word << " letters" << endl;
    }

public:
    Translate(const char* tra):
        max_in_alph(0),
        classified_symbols(255, make_pair(invalid_class, 0)) {
        prepare_fixed_defaults();
        prepare_default_hyfs();
        read_translate(tra);
    }

    Tindex get_max_in_alph(void) {
        return max_in_alph;
    }

    Tindex get_right_hyphen_min(void) {
        return right_hyphen_min;
    }

    Tindex get_left_hyphen_min(void) {
        return left_hyphen_min;
    }

    Tfile_unit get_edge_of_word(void) {
        return edge_of_word;
    }

    void get_xdig(Tnum_type i, basic_string<Tfile_unit>& e) {
        basic_string<Tfile_unit> inv_rep;
        while (i > 0) {
            xdig.add_to_string((i % 10), inv_rep);
            i = Tnum_type(i / 10);
        }
        e.append(inv_rep.rbegin(), inv_rep.rend());
    }

    void get_xhyf(const typename THword::hyphenation_type& i, basic_string<Tfile_unit>& e) {
        xhyf.add_to_string(i, e);
    }

    void get_xext(const Tnum_type& i, basic_string<Tfile_unit>& e) {
        xext.add_to_string(i, e);
    }
};

    
} // namespace ptl