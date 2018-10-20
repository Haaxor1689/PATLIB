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
#include "word_output_file.hpp"
#include "pattern_input_file.hpp"
#include "pattern_output_file.hpp"

const char* opatgen_version = "1.0";
const char* opatgen_cvs_id = "$Id: opatgen.w,v 1.24 2001/12/03 17:51:13 antos Exp $";

using namespace std;
using namespace ptl;

bool utf_8;

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

typedef word_output_file<Tindex, THword, TTranslate>
TWord_output_file;

typedef pattern_input_file<Tindex, Tin_alph, Tval_type, TTranslate,
                           TOutputs_of_a_pattern> TPattern_input_file;

typedef pattern_output_file<Tindex, Tin_alph, Tval_type, TTranslate,
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
