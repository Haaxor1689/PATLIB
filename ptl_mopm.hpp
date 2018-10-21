#pragma once

#include <iostream>
#include <map>
#include <set>

#include "typedefs.hpp"
#include "trie_pattern_manipulator.hpp"
#include "char_class.hpp"

namespace ptl {

using outputs_of_a_pattern = std::multimap<Tcount_good, Tcount_bad>;

class outputs_of_patterns : public std::set<outputs_of_a_pattern> {
    
    classified_symbol empty_iter;

public:
    outputs_of_patterns() : empty_iter(*insert(outputs_of_a_pattern()).first->begin()) {}

    classified_symbol get_empty_iter() const {
        return empty_iter;
    }

    void print_statistics() const {
        std::cout << "  number of different outputs:     " << size() << std::endl;
    }
};

class multi_output_pattern_manipulator {
protected:

    outputs_of_patterns outputs;
    trie_pattern_manipulator words;

public:
    multi_output_pattern_manipulator(const Tin_alph& max_i_a) : words(max_i_a, outputs.get_empty_iter()) {}

    multi_output_pattern_manipulator(multi_output_pattern_manipulator& old) : words(old.get_max_in_alph(), outputs.get_empty_iter()) {
        std::vector<Tin_alph> w;
        outputs_of_a_pattern o;

        old.init_walk_through();
        while (old.get_next_pattern(w, o))
            for (const auto& i : o)
                insert_pattern(w, i.first, i.second);
    }

    virtual ~multi_output_pattern_manipulator() {}

    virtual std::size_t get_max_in_alph() const {
        return words.get_max_in_alph();
    }

    virtual std::size_t get_pat_count() {
        return words.get_pat_count();
    }

    virtual void init_walk_through() {
        words.init_walk_through();
    }

    virtual bool get_next_pattern(std::vector<Tin_alph>& w, outputs_of_a_pattern& o) {
        classified_symbol i;
        bool ret_val = words.get_next_pattern(w, i);
        o = { i };
        return ret_val;
    }

    void word_output(const std::vector<Tin_alph>& w, outputs_of_a_pattern& o) {
        std::vector<classified_symbol> out_pointers;
        words.word_output(w, out_pointers);

        o.clear();
        for (const auto& i : out_pointers) {
            o.insert(i);
        }
    }

    void word_last_output(const std::vector<Tin_alph>& w, outputs_of_a_pattern& o) {
        classified_symbol i;
        words.word_last_output(w, i);
        o = { i };
    }

    void insert_pattern(const std::vector<Tin_alph>& w, Tcount_good p, const Tout_alph& v, bool with_erase = false) {
        outputs_of_a_pattern o;

        word_last_output(w, o);
        if (with_erase) {
            o.erase(p);
        }
        o.insert(std::make_pair(p, v));

        words.hard_insert_pattern(w, *outputs.insert(o).first->begin());
    }

    void delete_values() {
        std::vector<Tin_alph> w;
        outputs_of_a_pattern o;
        outputs_of_a_pattern n;

        init_walk_through();
        while (get_next_pattern(w, o)) {
            n.clear();
            for (const auto& i : o) {
                n.insert(i);
            }
            words.hard_insert_pattern(w, *outputs.insert(n).first->begin());
        }
    }

    void delete_position(const std::vector<Tin_alph>& w, Tcount_good p) {
        outputs_of_a_pattern o;

        word_last_output(w, o);
        o.erase(p);
        words.hard_insert_pattern(w, *outputs.insert(o).first->begin());
    }

    void delete_pattern(std::vector<Tin_alph>& w) {
        words.hard_insert_pattern(w, outputs.get_empty_iter());
    }

    virtual void delete_hanging() {
        words.delete_hanging();
    }

    void print_statistics() const {
        words.print_statistics();
        outputs.print_statistics();
    }

};

class competitive_multi_out_pat_manip : public multi_output_pattern_manipulator {

    using base = multi_output_pattern_manipulator;

public:
    competitive_multi_out_pat_manip(const Tin_alph& max_i_a) : base(max_i_a) {}

    competitive_multi_out_pat_manip(competitive_multi_out_pat_manip& old) : base(old) {}

    ~competitive_multi_out_pat_manip() override {}

    void competitive_pattern_output(const std::vector<Tin_alph>& w, outputs_of_a_pattern& o, const Tout_alph& ignore_bigger) {
        o.clear();
        std::size_t s = 0;
        for (auto i = w.begin(); i != w.end(); ++i) {
            std::vector<Tin_alph> v(i, w.end());
            competitive_word_output(v, o, s, ignore_bigger);
            ++s;
        }
    }

protected:
    void competitive_word_output(const std::vector<Tin_alph>& w, outputs_of_a_pattern& o, std::size_t s, const Tout_alph& ignore_bigger) {
        std::vector<classified_symbol> out_pointers;
        words.word_output(w, out_pointers);
        
        for (const auto& j : out_pointers) {
            if (j.second >= ignore_bigger)
                continue;
            auto oi = o.find(s + j.first);
            if (oi == o.end())
                o.insert(std::make_pair(s + j.first, j.second));
            else {
                if (oi->second < j.second) {
                    o.erase(s + j.first);
                    o.insert(std::make_pair(s + j.first, j.second));
                }
            }
        }
    }

};

} // namespace ptl
