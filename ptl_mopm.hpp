/*41:*/
#line 1066 "patlib.w"

#ifndef PTL_MOPM_H
#define PTL_MOPM_H

#include <iostream>
#include <map>
#include <set>
#include "ptl_tpm.hpp"

/*42:*/
#line 1103 "patlib.w"

template <class Tposition, class Toutput>
class Outputs_of_a_pattern :
        public std::multimap<Tposition, Toutput> {};

/*:42*/
#line 1075 "patlib.w"

/*43:*/
#line 1133 "patlib.w"

template <class Tposition, class Toutput>
class Outputs_of_patterns :
        public std::set<Outputs_of_a_pattern<Tposition, Toutput>> {
private:
    typename Outputs_of_patterns<Tposition, Toutput>::iterator empty_iter;

public:
    Outputs_of_patterns(void) {
        Outputs_of_a_pattern<Tposition, Toutput> empty;
        empty_iter = (this->insert(empty)).first;
    }

    typename Outputs_of_patterns<Tposition,
                                 Toutput>::iterator get_empty_iter(void) const {
        return empty_iter;
    }

    void print_statistics() const {
        cout << "  number of different outputs:     " << size() << endl;
    }
};

/*:43*/
#line 1076 "patlib.w"

/*44:*/
#line 1188 "patlib.w"

template <class Tindex, class Tin_alph, class Tout_alph>
class Multi_output_pattern_manipulator {
    /*45:*/
#line 1212 "patlib.w"

protected:
    typedef typename Outputs_of_patterns<Tindex, Tout_alph>::iterator Tout_iter;
    Outputs_of_patterns<Tindex, Tout_alph> outputs;
    Trie_pattern_manipulator<Tindex, Tin_alph, Tout_iter> words;

    /*:45*/
#line 1192 "patlib.w"

    /*46:*/
#line 1236 "patlib.w"

public:
    Multi_output_pattern_manipulator(const Tin_alph& max_i_a):
        outputs(),
        words(max_i_a, outputs.get_empty_iter()) {}

    Multi_output_pattern_manipulator(Multi_output_pattern_manipulator& old):
        outputs(),
        words(old.get_max_in_alph(), outputs.get_empty_iter()) {
        vector<Tin_alph> w;
        Outputs_of_a_pattern<Tindex, Tout_alph> o;

        old.init_walk_through();
        while (old.get_next_pattern(w, o))
            for (typename Outputs_of_a_pattern<Tindex, Tout_alph>::iterator i = o.begin();
                 i != o.end(); ++i)
                this->insert_pattern(w, i->first, i->second);
    }

    virtual ~Multi_output_pattern_manipulator() {}

    /*:46*/
#line 1193 "patlib.w"

    /*47:*/
#line 1265 "patlib.w"

public:
    virtual Tindex get_max_in_alph(void) const {
        return words.get_max_in_alph();
    }

    virtual Tindex get_pat_count(void) {
        return words.get_pat_count();
    }

    /*:47*/
#line 1194 "patlib.w"

    /*48:*/
#line 1290 "patlib.w"

public:
    virtual void init_walk_through(void) {
        words.init_walk_through();
    }

    virtual bool get_next_pattern(vector<Tin_alph>& w,
                                  Outputs_of_a_pattern<Tindex, Tout_alph>
                                  & o) {
        bool ret_val;
        typename Outputs_of_patterns<Tindex, Tout_alph>::iterator i;

        ret_val = words.get_next_pattern(w, i);
        o = *i;
        return ret_val;
    }

    /*:48*/
#line 1196 "patlib.w"

    /*49:*/
#line 1325 "patlib.w"

public:
    void word_output(const vector<Tin_alph>& w,
                     Outputs_of_a_pattern<Tindex, Tout_alph>& o) {
        vector<Tout_iter> out_pointers;
        words.word_output(w, out_pointers);

        o.clear();
        for (vector<Tout_iter>::const_iterator i = out_pointers.begin();
             i != out_pointers.end(); i++) {
            for (typename Outputs_of_a_pattern<Tindex, Tout_alph>::const_iterator j =
                         (*i)->begin(); j != (*i)->end(); ++j) {
                o.insert(*j);
            }
        }
    }

    /*:49*/
#line 1197 "patlib.w"

    /*50:*/
#line 1348 "patlib.w"

public:
    void word_last_output(const vector<Tin_alph>& w,
                          Outputs_of_a_pattern<Tindex, Tout_alph>& o) {
        typename Outputs_of_patterns<Tindex, Tout_alph>::iterator i;

        words.word_last_output(w, i);
        o = *i;
    }

    /*:50*/
#line 1198 "patlib.w"

    /*51:*/
#line 1368 "patlib.w"

public:
    void insert_pattern(const vector<Tin_alph>& w, const Tindex& p,
                        const Tout_alph& v, bool with_erase = false) {
        Outputs_of_a_pattern<Tindex, Tout_alph> o;

        word_last_output(w, o);
        if (with_erase)
            o.erase(p);
        o.insert(make_pair(p, v));

        words.hard_insert_pattern(w, outputs.insert(o).first);
    }

    /*:51*/
#line 1199 "patlib.w"

    /*52:*/
#line 1391 "patlib.w"

public:
    void delete_values(const Tout_alph& v) {
        vector<Tin_alph> w;
        Outputs_of_a_pattern<Tindex, Tout_alph> o;
        Outputs_of_a_pattern<Tindex, Tout_alph> n;

        init_walk_through();
        while (get_next_pattern(w, o)) {
            n.clear();
            for (typename Outputs_of_a_pattern<Tindex, Tout_alph>::iterator i = o.begin();
                 i != o.end(); ++i)
                if (i->second != v)
                    n.insert(*i);
            words.hard_insert_pattern(w, outputs.insert(n).first);
        }
    }

    /*:52*/
#line 1200 "patlib.w"

    /*53:*/
#line 1412 "patlib.w"

public:
    void delete_position(const vector<Tin_alph>& w, const Tindex& p) {
        Outputs_of_a_pattern<Tindex, Tin_alph> o;

        word_last_output(w, o);
        o.erase(p);
        words.hard_insert_pattern(w, outputs.insert(o).first);
    }

    /*:53*/
#line 1201 "patlib.w"

    /*54:*/
#line 1425 "patlib.w"

public:
    void delete_pattern(vector<Tin_alph>& w) {
        words.hard_insert_pattern(w, outputs.get_empty_iter());
    }

    /*:54*/
#line 1202 "patlib.w"

    /*55:*/
#line 1440 "patlib.w"

public:
    virtual void delete_hanging(void) {
        words.delete_hanging();
    }

    /*:55*/
#line 1203 "patlib.w"

    /*56:*/
#line 1449 "patlib.w"

public:
    void print_statistics(void) const {
        words.print_statistics();
        outputs.print_statistics();
    }

    /*:56*/
#line 1204 "patlib.w"

};

/*:44*/
#line 1077 "patlib.w"

/*57:*/
#line 1478 "patlib.w"

template <class Tindex, class Tin_alph, class Tout_alph>
class Competitive_multi_out_pat_manip :
        public Multi_output_pattern_manipulator<Tindex, Tin_alph, Tout_alph> {
    /*58:*/
#line 1492 "patlib.w"

public:
    Competitive_multi_out_pat_manip(const Tin_alph& max_i_a):
        Multi_output_pattern_manipulator<Tindex, Tin_alph, Tout_alph>(max_i_a) {}

    Competitive_multi_out_pat_manip(Competitive_multi_out_pat_manip& old):
        Multi_output_pattern_manipulator<Tindex, Tin_alph, Tout_alph>(old) {}

    ~Competitive_multi_out_pat_manip() override {}

    /*:58*/
#line 1483 "patlib.w"

    /*59:*/
#line 1522 "patlib.w"

protected:
    void competitive_word_output(const vector<Tin_alph>& w,
                                 Outputs_of_a_pattern<Tindex, Tout_alph>& o,
                                 const Tindex& s,
                                 const Tout_alph& ignore_bigger) {
        vector<Tout_iter> out_pointers;
        typename Outputs_of_a_pattern<Tindex, Tout_alph>::iterator oi;
        words.word_output(w, out_pointers);

        for (vector<Tout_iter>::const_iterator i = out_pointers.begin();
             i != out_pointers.end(); i++) {
            for (typename Outputs_of_a_pattern<Tindex, Tout_alph>::const_iterator j =
                         (*i)->begin(); j != (*i)->end(); ++j) {
                if (j->second >= ignore_bigger)
                    continue;
                oi = o.find(s + (j->first));
                if (oi == o.end())
                    o.insert(make_pair(s + (j->first), j->second));
                else {
                    if (oi->second < j->second) {
                        o.erase(s + (j->first));
                        o.insert(make_pair(s + (j->first), j->second));
                    }
                }
            }
        }
    }

    /*:59*/
#line 1484 "patlib.w"

    /*60:*/
#line 1562 "patlib.w"

public:
    void competitive_pattern_output(const vector<Tin_alph>& w,
                                    Outputs_of_a_pattern<Tindex, Tout_alph>& o,
                                    const Tout_alph& ignore_bigger) {
        o.clear();
        Tindex s = 0;
        for (vector<Tin_alph>::const_iterator i = w.begin(); i != w.end();
             i++) {
            vector<Tin_alph> v(i, w.end());
            competitive_word_output(v, o, s, ignore_bigger);
            ++s;
        }
    }

    /*:60*/
#line 1485 "patlib.w"

};

/*:57*/
#line 1078 "patlib.w"

#endif

/*:41*/
