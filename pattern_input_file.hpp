#pragma once

namespace ptl {
    
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

} // namespace ptl
