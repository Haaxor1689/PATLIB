#include <iostream>
#include <vector>
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
#include "translate.hpp"
#include "word_input_file.hpp"

const char* opatgen_version = "1.0";
const char* opatgen_cvs_id = "$Id: opatgen.w,v 1.24 2001/12/03 17:51:13 antos Exp $";

using namespace std;
using namespace ptl;

bool utf_8;
template <class Tindex, class Tin_alph, class Tval_type,
          class TTranslate, class TOutputs_of_a_pattern>
class Pattern_input_file {

protected:
    TTranslate& translate;
    const char* file_name;
    ifstream file;

    unsigned lineno;

    typedef typename TTranslate::Tfile_unit Tfile_unit;
    typedef typename TTranslate::classified_symbol Tclassified_symbol;

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
                if (i_class.first == char_class::letter) {
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
                case char_class::space:
                    goto done;
                case char_class::digit:

                    num = 0;
                    while (i_class.first == char_class::digit) {
                        num = 10 * num + i_class.second;
                        ++i;
                        translate.classify(*i, i_class);
                    }

                    o.insert(make_pair(chars_read, num));

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

typedef translate<Tindex, Tin_alph> TTranslate;

typedef candidate_count_trie<Tindex, Tin_alph, Tcount_type, Tcount_type>
TCandidate_count_structure;

typedef Competitive_multi_out_pat_manip<Tindex, Tin_alph, Tval_type>
TCompetitive_multi_out_pat_manip;

typedef Outputs_of_a_pattern<Tindex, Tval_type> TOutputs_of_a_pattern;

typedef word_input_file<THword, TTranslate, Tnum_type> TWord_input_file;

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
