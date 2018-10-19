#pragma once

#include "trie_pattern_manipulator.hpp"

namespace ptl {

template <class Tpm_pointer, class Tin_alph,
          class Tcount_good, class Tcount_bad>
class Candidate_count_trie :
        public Trie_pattern_manipulator<Tpm_pointer, Tin_alph,
                                        pair<Tcount_good, Tcount_bad>> {
public:
    typedef pair<Tcount_good, Tcount_bad> Tcount_pair;
    /*38:*/
#line 1005 "patlib.w"

public:
    Candidate_count_trie(const Tin_alph& max_i_a,
                         const Tcount_good& out_i_z_good,
                         const Tcount_bad& out_i_z_bad,
                         const unsigned& q_thr = 3):
        Trie_pattern_manipulator<Tpm_pointer, Tin_alph, Tcount_pair>
        (max_i_a, make_pair(out_i_z_good, out_i_z_bad), q_thr) {}

    /*:38*/
#line 996 "patlib.w"

    /*39:*/
#line 1025 "patlib.w"

public:
    virtual void increment_counts(const std::vector<Tin_alph>& w,
                                  const Tcount_good& good_inc,
                                  const Tcount_bad& bad_inc) {
        Tcount_pair counts;

        word_last_output(w, counts);
        counts.first += good_inc;
        counts.second += bad_inc;
        hard_insert_pattern(w, counts);

    }

    /*:39*/
#line 997 "patlib.w"

    /*40:*/
#line 1045 "patlib.w"

public:
    virtual bool get_next_pattern(std::vector<Tin_alph>& w,
                                  Tcount_good& good, Tcount_bad& bad) {
        Tcount_pair counts;
        bool ret_val;

        ret_val = Trie_pattern_manipulator<Tpm_pointer, Tin_alph,
                                           pair<Tcount_good, Tcount_bad>>::get_next_pattern(w, counts);
        good = counts.first;
        bad = counts.second;
        return ret_val;
    }

    /*:40*/
#line 998 "patlib.w"

};

/*:37*/
#line 262 "patlib.w"

#endif

} // namespace ptl
