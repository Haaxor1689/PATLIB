#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <fstream>

#include "candidate_count_trie.hpp"
#include "exception.hpp"
#include "generator.hpp"
#include "hyphenated_word.hpp"
#include "pass.hpp"
#include "level.hpp"
#include "ptl_mopm.hpp"
#include "version.hpp"
#include "IO_word_manipulator.hpp"

const char* opatgen_version = "1.0";
const char* opatgen_cvs_id = "$Id: opatgen.w,v 1.24 2001/12/03 17:51:13 antos Exp $";

using namespace std;
using namespace ptl;

bool utf_8;

template <class Texternal, class Tinternal>
class IO_reverse_mapping {
protected:
    map<Tinternal, vector<Texternal>> mapping;

public:
    void insert(const Tinternal& i, const vector<Texternal>& v) {
        mapping[i] = v;
    }

    void add_to_string(const Tinternal& i, basic_string<Texternal>& s) {
        typename map<Tinternal, vector<Texternal>>::const_iterator it = mapping.find(i);
        s.insert(s.end(), it->second.begin(), it->second.end());
    }

};

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

    IO_word_manipulator<Tindex, Tfile_unit, Tclassified_symbol> classified_symbols;

    IO_reverse_mapping<Tfile_unit, Tnum_type> xdig;
    IO_reverse_mapping<Tfile_unit, typename THword::hyphenation_type> xhyf;
    IO_reverse_mapping<Tfile_unit, Tnum_type> xext;

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

template <class THword, class TTranslate, class Tnum_type>
class Word_input_file {

protected:
    TTranslate& translate;
    const char* file_name;
    ifstream file;

    unsigned lineno;

    typedef typename TTranslate::Tfile_unit Tfile_unit;
    typedef typename TTranslate::Tclassified_symbol Tclassified_symbol;

    Tnum_type global_word_wt;

public:
    Word_input_file(TTranslate& t, const char* fn):
        translate(t), file_name(fn), file(file_name), lineno(0),
        global_word_wt(1) {}

protected:
    void handle_line(const basic_string<Tfile_unit>& s, THword& hw) {
        hw.push_back(translate.get_edge_of_word());
        hw.weight[hw.size()] = global_word_wt;

        Tclassified_symbol i_class;
        typename basic_string<Tfile_unit>::const_iterator i = s.begin();
        vector<Tfile_unit> seq;
        Tnum_type num;

        do {
            if (utf_8 && (*i & 0x80)) {
                {
                    Tfile_unit first_i = *i;
                    seq.clear();
                    while ((first_i & 0x80) && (*i & 0x80)) {

                        seq.push_back(*i);
                        ++i;
                        first_i = first_i << 1;
                    }
                    translate.classify(seq, i_class);
                    if (i_class.first == TTranslate::letter_class) {
                        hw.push_back(i_class.second);
                        hw.weight[hw.size()] = global_word_wt;
                    } else {
                        cerr << "! Error in " << file_name << " line " << lineno << ": "
                                << "Multibyte sequence is invalid" << endl;
                        throw ptl::exception("");
                    }
                }
            } else {
                translate.classify(*i, i_class);
                switch (i_class.first) {
                case TTranslate::space_class:
                    goto done;
                case TTranslate::digit_class:
                    if (i == s.begin()) {
                        num = 0;
                        while (i_class.first == TTranslate::digit_class) {
                            num = 10 * num + i_class.second;
                            ++i;
                            translate.classify(*i, i_class);
                        }
                        hw.weight[hw.size()] = num;
                        global_word_wt = num;
                    } else {
                        num = 0;
                        while (i_class.first == TTranslate::digit_class) {
                            num = 10 * num + i_class.second;
                            ++i;
                            translate.classify(*i, i_class);
                        }
                        hw.weight[hw.size()] = num;
                    }
                    break;
                case TTranslate::hyf_class:
                    if (i_class.second == THword::correct || i_class.second == THword::past)
                        hw.type[hw.size()] = THword::correct;
                    ++i;
                    break;
                case TTranslate::letter_class:
                    hw.push_back(i_class.second);
                    hw.weight[hw.size()] = global_word_wt;
                    ++i;
                    break;
                case TTranslate::escape_class:

                    seq.clear();
                    seq.push_back(*i);

                    ++i;
                    translate.classify(*i, i_class);
                    while (i_class.first == TTranslate::letter_class ||
                           i_class.first == TTranslate::invalid_class) {
                        seq.push_back(*i);
                        ++i;
                        translate.classify(*i, i_class);
                    }

                    while (i_class.first == TTranslate::space_class && i != s.end()) {
                        ++i;
                        translate.classify(*i, i_class);
                    }
                    translate.classify(seq, i_class);

                    if (i_class.first == TTranslate::letter_class) {
                        hw.push_back(i_class.second);
                        hw.weight[hw.size()] = global_word_wt;
                    } else {
                        cerr << "! Error in " << file_name << " line " << lineno << ": "
                                << "Escape sequence is invalid" << endl;
                        cerr << "(Are you using correct encoding--the -u8 switch?)" << endl;
                        throw ptl::exception("");
                    }

                    break;
                default:
                    cerr << "! Error in " << file_name << " line " << lineno << ": "
                            << "Invalid character in input data" << endl;
                    throw ptl::exception("");
                    break;
                }
            }
        } while (i != s.end());
    done:
        hw.push_back(translate.get_edge_of_word());
        hw.weight[hw.size()] = global_word_wt;
        hw.weight[0] = global_word_wt;
    }

public:
    bool get(THword& hw) {
        hw.clear();
        basic_string<Tfile_unit> s;

        if (!getline(file, s)) {
            return false;
        }
        lineno++;
        s.push_back(Tfile_unit(' '));
        handle_line(s, hw);
        return true;
    }

};

template <class Tindex, class Tin_alph, class Tval_type,
          class TTranslate, class TOutputs_of_a_pattern>
class Pattern_input_file {

protected:
    TTranslate& translate;
    const char* file_name;
    ifstream file;

    unsigned lineno;

    typedef typename TTranslate::Tfile_unit Tfile_unit;
    typedef typename TTranslate::Tclassified_symbol Tclassified_symbol;

public:
    Pattern_input_file(TTranslate& t, const char* fn):
        translate(t), file_name(fn), file(file_name), lineno(0) {}

protected:
    void handle_line(const basic_string<Tfile_unit>& s, vector<Tin_alph>& v,
                     TOutputs_of_a_pattern& o) {
        Tclassified_symbol i_class;
        typename basic_string<Tfile_unit>::const_iterator i = s.begin();
        vector<Tfile_unit> seq;
        Tval_type num;

        Tindex chars_read = 0;
        do {
            if (*i == '.') {
                v.push_back(translate.get_edge_of_word());
                ++chars_read;
                ++i;
                continue;
            }
            if (utf_8 && *i > 127) {

                Tfile_unit first_i = *i;
                seq.clear();
                while ((first_i & 0x80) && (*i & 0x80)) {

                    seq.push_back(*i);
                    ++i;
                    first_i = first_i << 1;
                }

                translate.classify(seq, i_class);
                if (i_class.first == TTranslate::letter_class) {
                    v.push_back(i_class.second);
                    ++chars_read;
                } else {
                    cerr << "! Error in " << file_name << " line " << lineno << ": "
                            << "Multibyte sequence is invalid" << endl;
                    throw ptl::exception("");
                }
            } else {
                translate.classify(*i, i_class);
                switch (i_class.first) {
                case TTranslate::space_class:
                    goto done;
                case TTranslate::digit_class:

                    num = 0;
                    while (i_class.first == TTranslate::digit_class) {
                        num = 10 * num + i_class.second;
                        ++i;
                        translate.classify(*i, i_class);
                    }

                    o.insert(make_pair(chars_read, num));

                    break;
                case TTranslate::letter_class:

                    v.push_back(i_class.second);
                    ++chars_read;
                    ++i;

                    break;
                case TTranslate::escape_class:

                    seq.clear();
                    seq.push_back(*i);

                    ++i;
                    translate.classify(*i, i_class);
                    while (i_class.first == TTranslate::letter_class ||
                           i_class.first == TTranslate::invalid_class) {
                        seq.push_back(*i);
                        ++i;
                        translate.classify(*i, i_class);
                    }

                    while (i_class.first == TTranslate::space_class && i != s.end()) {
                        ++i;
                        translate.classify(*i, i_class);
                    }
                    translate.classify(seq, i_class);

                    if (i_class.first == TTranslate::letter_class) {
                        v.push_back(i_class.second);
                        ++chars_read;
                    } else {
                        cerr << "! Error in " << file_name << " line " << lineno << ": "
                                << "Escape sequence is invalid" << endl;
                        cerr << "(Are you using correct encoding--the -u8 switch?)" << endl;
                        throw ptl::exception("");
                    }

                    break;
                default:
                    cerr << "! Error in " << file_name << " line " << lineno << ": "
                            << "Invalid character in pattern data" << endl;
                    throw ptl::exception("");
                }
            }
        } while (i != s.end());
    done:;
    }

public:
    bool get(vector<Tin_alph>& v, TOutputs_of_a_pattern& o) {
        v.clear();
        o.clear();
        basic_string<Tfile_unit> s;

        if (!getline(file, s)) {
            return false;
        }
        lineno++;
        s.push_back(' ');
        handle_line(s, v, o);
        return true;
    }

};

template <class Tindex, class THword, class TTranslate>
class Word_output_file {

protected:
    TTranslate& translate;
    const char* file_name;
    ofstream file;

    typedef typename TTranslate::Tfile_unit Tfile_unit;

    unsigned last_global_word_wt;
    unsigned global_word_wt;

public:
    Word_output_file(TTranslate& t, const char* fn):
        translate(t), file_name(fn), file(file_name), last_global_word_wt(1) {}

public:
    void put(THword& hw) {
        basic_string<Tfile_unit> s;

        global_word_wt = hw.weight[0];
        if (last_global_word_wt != global_word_wt) {
            translate.get_xdig(hw.weight[0], s);
            last_global_word_wt = global_word_wt;
        }

        if (hw.type[1] != THword::none)
            translate.get_xhyf(hw.type[1], s);

        for (Tindex dpos = 2; dpos <= hw.size() - 1; ++dpos) {
            translate.get_xext(hw[dpos], s);
            if (hw.type[dpos] != THword::none)
                translate.get_xhyf(hw.type[dpos], s);
            if (hw.weight[dpos] != global_word_wt)
                translate.get_xdig(hw.weight[dpos], s);
        }
        file << s << endl;
    }

};

template <class Tindex, class Tin_alph, class Tval_type,
          class TTranslate, class TOutputs_of_a_pattern>
class Pattern_output_file {

protected:
    TTranslate& translate;
    const char* file_name;
    ofstream file;

    typedef typename TTranslate::Tfile_unit Tfile_unit;

public:
    Pattern_output_file(TTranslate& t, const char* fn):
        translate(t), file_name(fn), file(file_name) {}

public:
    void put(const vector<Tin_alph>& v, const TOutputs_of_a_pattern& o) {
        typename TOutputs_of_a_pattern::const_iterator oi;
        basic_string<Tfile_unit> s;
        Tindex pos = 0;

        for (typename vector<Tin_alph>::const_iterator vi = v.begin();
             vi != v.end(); ++vi) {

            oi = o.find(pos);
            if (oi != o.end()) {
                translate.get_xdig(oi->second, s);
            }

            ++pos;
            translate.get_xext(*vi, s);
        }

        oi = o.find(pos);
        if (oi != o.end()) {
            translate.get_xdig(oi->second, s);
        }

        file << s << endl;
    }

};

typedef unsigned long Tindex;
typedef unsigned Tin_alph;
typedef unsigned short Tval_type;
typedef unsigned Twt_type;
typedef unsigned Tcount_type;
typedef unsigned Tnum_type;

typedef hyphenated_word<Tin_alph, Twt_type, Tval_type> THword;

typedef Translate<Tindex, Tin_alph, THword> TTranslate;

typedef candidate_count_trie<Tindex, Tin_alph, Tcount_type, Tcount_type>
TCandidate_count_structure;

typedef Competitive_multi_out_pat_manip<Tindex, Tin_alph, Tval_type>
TCompetitive_multi_out_pat_manip;

typedef Outputs_of_a_pattern<Tindex, Tval_type> TOutputs_of_a_pattern;

typedef Word_input_file<THword, TTranslate, Tnum_type> TWord_input_file;

typedef Word_output_file<Tindex, THword, TTranslate>
TWord_output_file;

typedef Pattern_input_file<Tindex, Tin_alph, Tval_type, TTranslate,
                           TOutputs_of_a_pattern> TPattern_input_file;

typedef Pattern_output_file<Tindex, Tin_alph, Tval_type, TTranslate,
                            TOutputs_of_a_pattern> TPattern_output_file;

typedef pass<Tindex, Tin_alph, Tval_type, Twt_type,
             Tcount_type, THword, TTranslate, TCandidate_count_structure,
             TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
             TWord_input_file> TPass;

typedef level<Tindex, Tin_alph, Tval_type, Twt_type,
              Tcount_type, THword, TTranslate, TCandidate_count_structure,
              TCompetitive_multi_out_pat_manip, TWord_input_file, TPass> TLevel;

void print_banner(void) {
    cout << endl;
    cout << "Written and maintained by David Antos, xantos (at) fi.muni.cz" << endl;
    cout << "Copyright (C) 2001 David Antos" << endl;
    cout << "This is free software; see the source for copying ";
    cout << "conditions. There is NO" << endl;
    cout << "warranty; not even for MERCHANTABILITY or FITNESS ";
    cout << "FOR A PARTICULAR PURPOSE." << endl << endl;
    cout << "Thank you for using free software!" << endl << endl;
}

int main(int argc, char* argv[]) {
    cout << "This is OPATGEN, version " << opatgen_version << endl;

    if (argc >= 2 && (0 == strcmp(argv[1], "--help"))) {
        cout << "Usage: opatgen [-u8] DICTIONARY PATTERNS OUTPUT TRANSLATE" << endl;
        cout << "  Generate the OUTPUT hyphenation file from the" << endl;
        cout << "  DICTIONARY, PATTERNS, and TRANSLATE files." << endl << endl;
        cout << "  -u8     files are in UTF-8 UNICODE encoding." << endl << endl;
        cout << "opatgen --help     print this help" << endl;
        cout << "opatgen --version  print version information" << endl;
        print_banner();
        return 0;
    }

    if (argc >= 2 && (0 == strcmp(argv[1], "--version"))) {
        cout << "(CVS: " << opatgen_cvs_id << ")" << endl;
        cout << "with PATLIB, version " << patlib_version << endl;
        cout << "(CVS: " << patlib_cvs_id << ")" << endl;
        print_banner();
        return 0;
    }

    print_banner();

    try {
        if (argc == 5) {
            utf_8 = false;
            generator<Tindex, Tin_alph, Tval_type, Twt_type, Tcount_type, THword,
                      TTranslate, TCandidate_count_structure,
                      TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
                      TWord_input_file, TWord_output_file,
                      TPattern_input_file, TPattern_output_file,
                      TPass, TLevel>
                    g(argv[1], argv[2], argv[3], argv[4]);
            g.do_all();
        } else if (argc == 6 && (0 == strcmp(argv[1], "-u8"))) {

            utf_8 = true;
            generator<Tindex, Tin_alph, Tval_type, Twt_type, Tcount_type, THword,
                      TTranslate, TCandidate_count_structure,
                      TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
                      TWord_input_file, TWord_output_file,
                      TPattern_input_file, TPattern_output_file,
                      TPass, TLevel>
                    g(argv[2], argv[3], argv[4], argv[5]);
            g.do_all();
        } else {
            cout << "opatgen: needs some arguments" << endl
                    << "Try `opatgen --help'" << endl;
            return 1;
        }
    } catch (const ptl::exception& e) {
        cerr << e.what() << endl;
        cerr << endl << "This was fatal error, sorry. Giving up." << endl;
    }
    catch (...) {
        cerr << "An unexpected exception occurred. It means there is probably" << endl;
        cerr << "a bug in the program. Please report it to the maintainer." << endl;
        cerr << "Use opatgen --version to find out who the maintainer is.";
        cout << "Do you want me to dump core? <y/n> " << endl;
        string s;
        cin >> s;
        if (s == "y" || s == "Y") {
            cout << endl << "Now I dump core..." << endl;
            terminate();
        }
    }
}

/*:61*/
