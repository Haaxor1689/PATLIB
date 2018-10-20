#include <iostream>
#include <string>

#include "exception.hpp"
#include "generator.hpp"
#include "version.hpp"

const char* opatgen_version = "1.0";
const char* opatgen_cvs_id = "$Id: opatgen.w,v 1.24 2001/12/03 17:51:13 antos Exp $";

using namespace ptl;

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
            generator g(argv[1], argv[2], argv[3], argv[4]);
            g.do_all();
        } else if (argc == 6 && (0 == strcmp(argv[1], "-u8"))) {

            utf_8 = true;
            generator g(argv[2], argv[3], argv[4], argv[5]);
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
