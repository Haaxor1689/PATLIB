#pragma once

#include <iostream>
#include <map>
#include <set>

#include "trie_pattern_manipulator.hpp"

namespace ptl {

template <class Tposition, class Toutput>
class Outputs_of_a_pattern : public std::multimap<Tposition, Toutput> {};

template <class Tposition, class Toutput>
class Outputs_of_patterns : public std::set<Outputs_of_a_pattern<Tposition, Toutput>> {
    using node_type = Outputs_of_a_pattern<Tposition, Toutput>;
    using base = std::set<node_type>;
    using node_iterator = typename node_type::iterator;
    
    node_iterator empty_iter;

public:
    Outputs_of_patterns() : empty_iter((base::insert(node_type())).first) {}

    auto get_empty_iter() const {
        return empty_iter;
    }

    void print_statistics() const {
        std::cout << "  number of different outputs:     " << base::size() << std::endl;
    }
};

template <class Tindex, class Tin_alph, class Tout_alph>
class Multi_output_pattern_manipulator {
protected:
    using output_type = Outputs_of_patterns<Tindex, Tout_alph>;
    using output_iterator = typename output_type::iterator;

    output_type outputs;
    trie_pattern_manipulator<Tindex, Tin_alph, output_iterator> words;

public:
    Multi_output_pattern_manipulator(const Tin_alph& max_i_a) : words(max_i_a, outputs.get_empty_iter()) {}

    Multi_output_pattern_manipulator(Multi_output_pattern_manipulator& old) : words(old.get_max_in_alph(), outputs.get_empty_iter()) {
        std::vector<Tin_alph> w;
        Outputs_of_a_pattern<Tindex, Tout_alph> o;

        old.init_walk_through();
        while (old.get_next_pattern(w, o))
            for (typename Outputs_of_a_pattern<Tindex, Tout_alph>::iterator i = o.begin();
                 i != o.end(); ++i)
                this->insert_pattern(w, i->first, i->second);
    }

    virtual ~Multi_output_pattern_manipulator() {}

    virtual Tindex get_max_in_alph() const {
        return words.get_max_in_alph();
    }

    virtual Tindex get_pat_count() {
        return words.get_pat_count();
    }

    virtual void init_walk_through() {
        words.init_walk_through();
    }

    virtual bool get_next_pattern(std::vector<Tin_alph>& w, Outputs_of_a_pattern<Tindex, Tout_alph>& o) {
        output_iterator i;
        bool ret_val = words.get_next_pattern(w, i);
        o = *i;
        return ret_val;
    }

    void word_output(const std::vector<Tin_alph>& w, Outputs_of_a_pattern<Tindex, Tout_alph>& o) {
        std::vector<output_iterator> out_pointers;
        words.word_output(w, out_pointers);

        o.clear();
        for (const auto& i : out_pointers) {
            for (const auto& j : i) {
                o.insert(j);
            }
        }
    }

    void word_last_output(const std::vector<Tin_alph>& w, Outputs_of_a_pattern<Tindex, Tout_alph>& o) {
        output_iterator i;
        words.word_last_output(w, i);
        o = *i;
    }

    void insert_pattern(const std::vector<Tin_alph>& w, const Tindex& p, const Tout_alph& v, bool with_erase = false) {
        Outputs_of_a_pattern<Tindex, Tout_alph> o;

        word_last_output(w, o);
        if (with_erase) {
            o.erase(p);
        }
        o.insert(std::make_pair(p, v));

        words.hard_insert_pattern(w, outputs.insert(o).first);
    }

    void delete_values(const Tout_alph& v) {
        std::vector<Tin_alph> w;
        Outputs_of_a_pattern<Tindex, Tout_alph> o;
        Outputs_of_a_pattern<Tindex, Tout_alph> n;

        init_walk_through();
        while (get_next_pattern(w, o)) {
            n.clear();
            for (const auto& i : o) {
                n.insert(i);
            }
            words.hard_insert_pattern(w, outputs.insert(n).first);
        }
    }

    void delete_position(const std::vector<Tin_alph>& w, const Tindex& p) {
        Outputs_of_a_pattern<Tindex, Tin_alph> o;

        word_last_output(w, o);
        o.erase(p);
        words.hard_insert_pattern(w, outputs.insert(o).first);
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

template <class Tindex, class Tin_alph, class Tout_alph>
class Competitive_multi_out_pat_manip : public Multi_output_pattern_manipulator<Tindex, Tin_alph, Tout_alph> {
    using base = Multi_output_pattern_manipulator<Tindex, Tin_alph, Tout_alph>;

public:
    Competitive_multi_out_pat_manip(const Tin_alph& max_i_a) : base(max_i_a) {}

    Competitive_multi_out_pat_manip(Competitive_multi_out_pat_manip& old) : base(old) {}

    ~Competitive_multi_out_pat_manip() override {}

    void competitive_pattern_output(const std::vector<Tin_alph>& w, Outputs_of_a_pattern<Tindex, Tout_alph>& o, const Tout_alph& ignore_bigger) {
        o.clear();
        Tindex s = 0;
        for (typename std::vector<Tin_alph>::const_iterator i = w.begin(); i != w.end(); ++i) {
            std::vector<Tin_alph> v(i, w.end());
            competitive_word_output(v, o, s, ignore_bigger);
            ++s;
        }
    }

protected:
    void competitive_word_output(const std::vector<Tin_alph>& w, Outputs_of_a_pattern<Tindex, Tout_alph>& o, const Tindex& s, const Tout_alph& ignore_bigger) {
        std::vector<typename base::output_iterator> out_pointers;
        base::words.word_output(w, out_pointers);
        
        for (const auto& i : out_pointers) {
            for (const auto& j : i) {
                if (j.second >= ignore_bigger)
                    continue;
                auto oi = o.find(s + (j.first));
                if (oi == o.end())
                    o.insert(make_pair(s + (j.first), j.second));
                else {
                    if (oi->second < j.second) {
                        o.erase(s + (j.first));
                        o.insert(make_pair(s + (j.first), j.second));
                    }
                }
            }
        }
    }

};

} // namespace ptl
