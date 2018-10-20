#pragma once

#include <iostream>
#include <vector>
#include <set>

#include "typedefs.hpp"
#include "growing_array.hpp"

namespace ptl {

    class trie_pattern_manipulator {

        enum {
            min_in_alph = 0
        };


        std::size_t trie_max;
        std::size_t trie_bmax;
        std::size_t trie_count;
        std::size_t pat_count;

        const Tin_alph max_in_alph;
        const Tcount_pair out_inf_zero;

        growing_array<Tin_alph> trie_char;
        growing_array<std::size_t> trie_link;
        growing_array<std::size_t> trie_back;
        growing_array<char> trie_base_used;

        unsigned q_max;
        unsigned q_max_thresh;
        Tin_alph* trieq_char;
        std::size_t* trieq_link;
        std::size_t* trieq_back;
        Tcount_pair* trieq_outp;
        
        std::vector<std::size_t> pointer_stack;
        std::vector<std::size_t> char_stack;

    protected:
        growing_array<Tcount_pair> trie_outp;
        enum { trie_root = 1 };

    public:
        trie_pattern_manipulator(const Tin_alph& max_i_a, const Tcount_pair& out_i_z, const unsigned& q_thr = 5) :
            max_in_alph(max_i_a), out_inf_zero(out_i_z),
            trie_char(min_in_alph), trie_link(min_in_alph),
            trie_back(min_in_alph), trie_base_used(false), trie_outp(out_inf_zero),
            q_max_thresh(q_thr) {
            for (std::size_t c = min_in_alph; c <= max_in_alph; ++c) {
                trie_char[trie_root + c] = c;
            }

            trie_base_used[trie_root] = true;
            trie_bmax = trie_root;
            trie_max = trie_root + max_in_alph;

            trie_count = max_in_alph + 1;
            pat_count = 0;

            trie_link[0] = trie_max + 1;
            trie_back[trie_max + 1] = 0;

            trieq_char = new Tin_alph[max_in_alph + 1];
            trieq_link = new std::size_t[max_in_alph + 1];
            trieq_back = new std::size_t[max_in_alph + 1];
            trieq_outp = new Tcount_pair[max_in_alph + 1];
        }

        virtual ~trie_pattern_manipulator() {
            delete[]trieq_char;
            delete[]trieq_link;
            delete[]trieq_back;
            delete[]trieq_outp;
        }

        unsigned get_q_max_thresh() const {
            return q_max_thresh;
        }

        void set_q_max_thresh(unsigned new_q_m_t) {
            if (new_q_m_t > 0)
                q_max_thresh = new_q_m_t;
        }

        std::size_t get_trie_count() const {
            return trie_count;
        }

        std::size_t get_pat_count() const {
            return pat_count;
        }

        std::size_t get_max_in_alph() const {
            return max_in_alph;
        }

    protected:
        std::size_t first_fit() {
            unsigned int q;
            std::size_t s, t;

            if (q_max > q_max_thresh) {
                t = trie_back[trie_max + 1];
            } else { t = 0; }
            while (true) {
                t = trie_link[t];
                s = t - trieq_char[1];

                while (trie_bmax < s) {
                    ++trie_bmax;
                    trie_base_used[trie_bmax] = false;
                    trie_char[trie_bmax + max_in_alph] = min_in_alph;
                    trie_outp[trie_bmax + max_in_alph] = out_inf_zero;

                    trie_link[trie_bmax + max_in_alph] = trie_bmax + max_in_alph + 1;
                    trie_back[trie_bmax + max_in_alph + 1] = trie_bmax + max_in_alph;
                }

                if (trie_base_used[s]) {
                    goto not_found;
                }
                for (q = q_max; q >= 2; q--) {
                    if (trie_char[s + trieq_char[q]] != min_in_alph) {
                        goto not_found;
                    }
                }
                goto found;
            not_found:;
            }
        found:;

            for (q = 1; q <= q_max; q++) {
                t = s + trieq_char[q];

                trie_link[trie_back[t]] = trie_link[t];
                trie_back[trie_link[t]] = trie_back[t];
                trie_char[t] = trieq_char[q];
                trie_link[t] = trieq_link[q];
                trie_back[t] = trieq_back[q];
                trie_outp[t] = trieq_outp[q];
                if (t > trie_max) { trie_max = t; }
            }
            trie_base_used[s] = true;
            return s;
        }

        void unpack(const std::size_t& s) {
            q_max = 1;
            for (std::size_t c = min_in_alph; c <= max_in_alph; ++c) {
                std::size_t t = s + c;
                if (trie_char[t] == c && c != min_in_alph) {

                    trieq_char[q_max] = c;
                    trieq_link[q_max] = trie_link[t];
                    trieq_back[q_max] = trie_back[t];
                    trieq_outp[q_max] = trie_outp[t];
                    q_max++;

                    trie_back[trie_link[0]] = t;
                    trie_link[t] = trie_link[0];
                    trie_link[0] = t;
                    trie_back[t] = 0;
                    trie_char[t] = min_in_alph;
                    trie_outp[t] = out_inf_zero;
                }
            }
            trie_base_used[s] = false;
        }

    public:
        virtual void hard_insert_pattern(const std::vector<Tin_alph>& w, const Tcount_pair& o) {
            if (w.empty()) {
                return;
            }

            auto i = w.begin();
            std::size_t s = trie_root + *i;
            std::size_t t = trie_link[s];

            while ((t > 0) && ((i + 1) != w.end())) {
                ++i;
                t += *i;
                if (trie_char[t] != *i) {

                    if (trie_char[t] == min_in_alph) {

                        trie_link[trie_back[t]] = trie_link[t];
                        trie_back[trie_link[t]] = trie_back[t];
                        trie_char[t] = *i;
                        trie_link[t] = 0;
                        trie_back[t] = 0;
                        trie_outp[t] = out_inf_zero;
                        if (t > trie_max) {
                            trie_max = t;
                        }
                    } else {

                        unpack(t - *i);
                        trieq_char[q_max] = *i;
                        trieq_link[q_max] = 0;
                        trieq_back[q_max] = 0;
                        trieq_outp[q_max] = out_inf_zero;
                        t = first_fit();
                        trie_link[s] = t;
                        t += *i;
                    }
                    ++trie_count;
                }

                s = t;
                t = trie_link[s];
            }

            trieq_link[1] = 0;
            trieq_back[1] = 0;
            trieq_outp[1] = out_inf_zero;
            q_max = 1;
            while ((i + 1) != w.end()) {
                ++i;
                trieq_char[1] = *i;
                t = first_fit();
                trie_link[s] = t;
                s = t + *i;
                ++trie_count;
            }

            if ((trie_outp[s] == out_inf_zero) && (o != out_inf_zero)) {
                ++pat_count;
            }

            if ((trie_outp[s] != out_inf_zero) && (o == out_inf_zero)) {
                --pat_count;
            }

            trie_outp[s] = o;
        }

        void init_walk_through() {
            pointer_stack.clear();
            char_stack.clear();
            pointer_stack.push_back(trie_root);
            char_stack.push_back(min_in_alph);
        }

        bool get_next_pattern(std::vector<Tin_alph>& w, Tcount_pair& o) {
            w.clear();
            o = out_inf_zero;
            while (true) {
                if (pointer_stack.empty())
                    return false;

                std::size_t tstart = pointer_stack.back();
                pointer_stack.pop_back();

                std::size_t cstart = char_stack.back();
                char_stack.pop_back();

                for (std::size_t c = cstart; c <= max_in_alph; ++c) {

                    std::size_t t = tstart + c;

                    if (trie_char[t] == c && c != min_in_alph) {

                        pointer_stack.push_back(tstart);
                        char_stack.push_back(c + 1);
                        if (trie_outp[t] != out_inf_zero) {
                            auto it = pointer_stack.begin();
                            auto ic = char_stack.begin();
                            std::size_t u;

                            for (std::size_t i = 0; i < pointer_stack.size(); ++i) {
                                u = (*it) + std::size_t(*ic) - 1;
                                w.push_back(trie_char[u]);
                                ++it;
                                ++ic;
                            }
                            o = trie_outp[u];

                            if (trie_link[t] != 0) {
                                pointer_stack.push_back(trie_link[t]);
                                char_stack.push_back(min_in_alph);
                            }
                            return true;
                        }

                        if (trie_link[t] != 0) {
                            pointer_stack.push_back(trie_link[t]);
                            char_stack.push_back(min_in_alph);
                        }
                        break;
                    }
                }
            }
        }

        virtual void word_output(const std::vector<Tin_alph>& w, std::vector<Tcount_pair>& o) {
            o.clear();

            if (w.empty()) {
                return;
            }
            
            auto i = w.begin();
            std::size_t t = trie_root;

            do {
                t += *i;
                if (trie_char[t] == *i)
                    o.push_back(trie_outp[t]);
                else
                    break;
                t = trie_link[t];
                ++i;
            } while (t != 0 && i != w.end());

            while (i != w.end()) {
                o.push_back(out_inf_zero);
                ++i;
            }
        }

        void word_last_output(const std::vector<Tin_alph>& w, Tcount_pair& o) {
            // todo What?
            #if 0==1
            o = out_inf_zero;
            std::vector<Tcount_pair> whole_o;
            word_output(w,whole_o);
            if(whole_o.size()>=1)o= *(whole_o.end()-1);
            #endif

            o = out_inf_zero;

            if (w.empty()) {
                return;
            }
            
            auto i = w.begin();
            std::size_t t = trie_root;
            std::size_t s;
            do {
                t += *i;
                if (trie_char[t] == *i)
                    s = t;
                else
                    break;
                t = trie_link[t];
                i++;
            } while (t != 0 && i != w.end());

            if (i == w.end()) {
                o = trie_outp[s];
            }
        }

    private:
        bool delete_hanging_level(const std::size_t& s) {
            bool all_freed = true;
            for (std::size_t c = min_in_alph; c <= max_in_alph; ++c) {

                std::size_t t = s + c;
                if (trie_char[t] == c && c != min_in_alph) {
                    if (trie_link[t] != 0 && delete_hanging_level(trie_link[t])) {
                        trie_link[t] = 0;
                    }
                    if ((trie_link[t] != 0) || (trie_outp[t] != out_inf_zero) || (s == trie_root)) {
                        all_freed = false;
                    } else {
                        trie_link[trie_back[trie_max + 1]] = t;
                        trie_back[t] = trie_back[trie_max + 1];
                        trie_link[t] = trie_max + 1;
                        trie_back[trie_max + 1] = t;
                        trie_char[t] = min_in_alph;
                        --trie_count;
                    }
                }
            }
            if (all_freed) {
                trie_base_used[s] = false;
            }
            return all_freed;
        }

    public:
        void delete_hanging() {
            delete_hanging_level(trie_root);
        }

        void set_of_my_outputs(std::set<Tcount_pair>& s) {
            s.clear();
            for (std::size_t i = 0; i <= trie_max; ++i) {
                s.insert(trie_outp[i]);
            }
        }

        void print_statistics(int detail = 0) const {
            std::cout << "  nodes:                           " << trie_count << std::endl;
            std::cout << "  patterns:                        " << pat_count << std::endl;
            std::cout << "  trie_max:                        " << trie_max << std::endl;
            std::cout << "  current q_max_thresh:            " << q_max_thresh << std::endl;
            if (detail) {
                std::cout << "Trie char" << std::endl;
                trie_char.print_statistics();
                std::cout << "Trie link" << std::endl;
                trie_link.print_statistics();
                std::cout << "Trie back" << std::endl;
                trie_back.print_statistics();
                std::cout << "Trie base used" << std::endl;
                trie_base_used.print_statistics();
                std::cout << "Trie outp" << std::endl;
                trie_outp.print_statistics();
            }
        }
    };

} // namespace ptl
