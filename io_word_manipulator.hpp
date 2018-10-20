#pragma once

#include <vector>

#include "trie_pattern_manipulator.hpp"

namespace ptl {

template <class Tpm_pointer, class Tin_alph, class Tout_information>
class io_word_manipulator : public trie_pattern_manipulator<Tpm_pointer, Tin_alph, Tout_information> {
    using base = trie_pattern_manipulator<Tpm_pointer, Tin_alph, Tout_information>;

public:
    io_word_manipulator(const Tin_alph& max_i_a, const Tout_information& out_i_z, const unsigned& q_thr = 3) : base(max_i_a, out_i_z, q_thr) {}

    void hard_insert_pattern(const Tin_alph& w, const Tout_information& o) override {
        std::vector<Tin_alph> vec;
        vec.push_back(w);

        base::hard_insert_pattern(vec, o);
    }

    void word_output(const Tin_alph& w, Tout_information& o) override {
        o = base::trie_outp[base::trie_root + w];
    }

};
    
} // namespace ptl