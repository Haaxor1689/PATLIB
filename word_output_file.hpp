#pragma once

namespace ptl {
    
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

} // namespace ptl
