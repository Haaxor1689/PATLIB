#include <iostream>
#include <string>

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

using namespace ptl;

typedef competitive_multi_out_pat_manip<std::size_t, unsigned, unsigned> TCompetitive_multi_out_pat_manip;

typedef outputs_of_a_pattern<std::size_t, unsigned> TOutputs_of_a_pattern;

typedef word_input_file<hyphenated_word, translate, unsigned> TWord_input_file;

typedef word_output_file<std::size_t, hyphenated_word, translate>
TWord_output_file;

typedef pattern_input_file<std::size_t, unsigned, unsigned, translate,
                           TOutputs_of_a_pattern> TPattern_input_file;

typedef pattern_output_file<std::size_t, unsigned, unsigned, translate,
                            TOutputs_of_a_pattern> TPattern_output_file;

typedef pass<std::size_t, unsigned, unsigned, unsigned,
             unsigned, hyphenated_word, translate, candidate_count_trie,
             TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
             TWord_input_file> TPass;

typedef level<std::size_t, unsigned, unsigned, unsigned,
              unsigned, hyphenated_word, translate, candidate_count_trie,
              TCompetitive_multi_out_pat_manip, TWord_input_file, TPass> TLevel;

void print_banner() {
    std::cout << std::endl;
    std::cout << "Written and maintained by David Antos, xantos (at) fi.muni.cz" << std::endl;
    std::cout << "Copyright (C) 2001 David Antos" << std::endl;
    std::cout << "This is free software; see the source for copying ";
    std::cout << "conditions. There is NO" << std::endl;
    std::cout << "warranty; not even for MERCHANTABILITY or FITNESS ";
    std::cout << "FOR A PARTICULAR PURPOSE." << std::endl << std::endl;
    std::cout << "Thank you for using free software!" << std::endl << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "This is OPATGEN, version " << opatgen_version << std::endl;

    if (argc >= 2 && (0 == strcmp(argv[1], "--help"))) {
        std::cout << "Usage: opatgen [-u8] DICTIONARY PATTERNS OUTPUT TRANSLATE" << std::endl;
        std::cout << "  Generate the OUTPUT hyphenation file from the" << std::endl;
        std::cout << "  DICTIONARY, PATTERNS, and TRANSLATE files." << std::endl << std::endl;
        std::cout << "  -u8     files are in UTF-8 UNICODE encoding." << std::endl << std::endl;
        std::cout << "opatgen --help     print this help" << std::endl;
        std::cout << "opatgen --version  print version information" << std::endl;
        print_banner();
        return 0;
    }

    if (argc >= 2 && (0 == strcmp(argv[1], "--version"))) {
        std::cout << "(CVS: " << opatgen_cvs_id << ")" << std::endl;
        std::cout << "with PATLIB, version " << patlib_version << std::endl;
        std::cout << "(CVS: " << patlib_cvs_id << ")" << std::endl;
        print_banner();
        return 0;
    }

    print_banner();
    
    bool utf_8;
    try {
        if (argc == 5) {
            utf_8 = false;
            generator<std::size_t, unsigned, unsigned, unsigned, unsigned, hyphenated_word,
                      translate, candidate_count_trie,
                      TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
                      TWord_input_file, TWord_output_file,
                      TPattern_input_file, TPattern_output_file,
                      TPass, TLevel>
                    g(argv[1], argv[2], argv[3], argv[4]);
            g.do_all();
        } else if (argc == 6 && (0 == strcmp(argv[1], "-u8"))) {

            utf_8 = true;
            generator<std::size_t, unsigned, unsigned, unsigned, unsigned, hyphenated_word,
                      translate, candidate_count_trie,
                      TCompetitive_multi_out_pat_manip, TOutputs_of_a_pattern,
                      TWord_input_file, TWord_output_file,
                      TPattern_input_file, TPattern_output_file,
                      TPass, TLevel>
                    g(argv[2], argv[3], argv[4], argv[5]);
            g.do_all();
        } else {
            std::cout << "opatgen: needs some arguments" << std::endl
                      << "Try `opatgen --help'" << std::endl;
            return 1;
        }
    } catch (const ptl::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl << "This was fatal error, sorry. Giving up." << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "An unexpected exception occurred. It means there is probably" << std::endl;
        std::cerr << "a bug in the program. Please report it to the maintainer." << std::endl;
        std::cerr << "Use opatgen --version to find out who the maintainer is.";
        return -1;
    }
    return 0;
}
