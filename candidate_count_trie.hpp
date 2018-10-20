#pragma once

#include "typedefs.hpp"
#include "trie_pattern_manipulator.hpp"

namespace ptl {

class candidate_count_trie : public trie_pattern_manipulator {

    using base = trie_pattern_manipulator;

public:
    candidate_count_trie(const Tin_alph& max_i_a, const Tcount_good& out_i_z_good, const Tcount_bad& out_i_z_bad, const unsigned& q_thr = 3) :
        base(max_i_a, std::make_pair(out_i_z_good, out_i_z_bad), q_thr) {}

    void increment_counts(const std::vector<Tin_alph>& w, const Tcount_good& good_inc, const Tcount_bad& bad_inc) {
        Tcount_pair counts;

        word_last_output(w, counts);
        counts.first += good_inc;
        counts.second += bad_inc;
        hard_insert_pattern(w, counts);
    }

    bool get_next_pattern(std::vector<Tin_alph>& w, Tcount_good& good, Tcount_bad& bad) {
        Tcount_pair counts;
        bool ret_val = get_next_pattern(w, counts.first, counts.second);
        good = counts.first;
        bad = counts.second;
        return ret_val;
    }

};

} // namespace ptl
