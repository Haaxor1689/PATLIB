#pragma once

namespace ptl {
    
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

} // namespace ptl
