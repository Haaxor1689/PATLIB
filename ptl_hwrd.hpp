/*72:*/
#line 1787 "patlib.w"

#ifndef PTL_HWRD_H
#define PTL_HWRD_H

#include <iostream>
#include <vector>
#include "ptl_ga.h"

/*73:*/
#line 1833 "patlib.w"

template <class Tindex, class Tin_alph, class Twt_type, class Tval_type>
class Hword :
        public std::vector<Tin_alph> {
public:
    typedef enum {
        no_hyf,
        err_hyf,
        is_hyf,
        found_hyf
    } Thyf_type;

    Growing_array<Tindex, Thyf_type> dots;
    Growing_array<Tindex, Twt_type> dotw;
    Growing_array<Tindex, Tval_type> hval;
    Growing_array<Tindex, char> no_more;

    /*74:*/
#line 1865 "patlib.w"

    Hword():
        std::vector<Tin_alph>(),
        dots(no_hyf), dotw(1), hval(0), no_more(false) {}

    Hword(const Thyf_type& s, const Twt_type& w, const Tval_type& l,
          const char& e):
        std::vector<Tin_alph>(),
        dots(s), dotw(w), hval(l), no_more(e) {}

    /*:74*/
#line 1848 "patlib.w"

    /*76:*/
#line 1891 "patlib.w"

public:
    void clear(void) {
        std::vector<Tin_alph>::clear();
        dots.clear();
        dotw.clear();
        hval.clear();
        no_more.clear();
    }

    /*:76*/
#line 1849 "patlib.w"

    /*75:*/
#line 1881 "patlib.w"

public:
    Tin_alph& operator[](const Tindex& i) {
        return std::vector<Tin_alph>::operator[](i - 1);
    }

    /*:75*/
#line 1850 "patlib.w"

#ifdef DEBUG
    /*77:*/
#line 1904 "patlib.w"

    void print(void) {
        cout << "Hword";
        for (typename std::vector<Tin_alph>::iterator i = this->begin(); i !=
                                                                         this->end(); ++i)
            cout << " " << *i;
        cout << endl << "dots";
        for (Tindex i = 0; i <= this->size(); ++i)
            cout << " " << dots[i];
        cout << endl << "dotw";
        for (Tindex i = 0; i <= this->size(); ++i)
            cout << " " << dotw[i];
        cout << endl << "hval";
        for (Tindex i = 0; i <= this->size(); ++i)
            cout << " " << hval[i];
        cout << endl << "no_m";
        for (Tindex i = 0; i <= this->size(); ++i)
            if (no_more[i])
                cout << " t";
            else
                cout << " f";
        cout << endl;
    }

    /*:77*/
#line 1852 "patlib.w"

#endif
};

/*:73*/
#line 1795 "patlib.w"

#endif

/*:72*/
