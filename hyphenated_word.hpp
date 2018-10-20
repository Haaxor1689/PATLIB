# pragma once

#include <iostream>
#include <vector>

#include "hyphenation_type.hpp"
#include "growing_array.hpp"

namespace ptl {

/**
 * \brief Class for holding info about hyphenated word.
 * \todo Remove 4 redundant growing arrays.
 */
class hyphenated_word : public std::vector<unsigned> {
    using base = std::vector<unsigned>;

    using Tin_alph = unsigned;
    using Twt_type = unsigned;
    using Tval_type = unsigned;
    
public:
    growing_array<hyphenation_type> type;
    growing_array<Twt_type> weight;
    growing_array<Tval_type> level;
    growing_array<bool> is_knocked_out;

    /**
     * \brief Constructs empty hyphenated word with default values
     */
    hyphenated_word() : base(), type(hyphenation_type::none), weight(1), level(0), is_knocked_out(false) {}

    /**
     * \brief Constructs empty hyphenated word with custom default values
     * \param s Default value for type
     * \param w Default value for weight
     * \param l Default value for level
     * \param e Default knocked out state
     */
    hyphenated_word(const hyphenation_type& s, const Twt_type& w, const Tval_type& l, bool e) : base(), type(s), weight(w), level(l), is_knocked_out(e) {}

    /**
     * \brief Clears hyphenated word and all it's growing arrays
     */
    void clear() {
        base::clear();
        type.clear();
        weight.clear();
        level.clear();
        is_knocked_out.clear();
    }
    
    /**
     * \brief Accesses word as if it was indexed from one.
     * \todo Scrap word indexing from one.
     * \param i Index.
     * \return Element at given index.
     */
    Tin_alph& operator[](const std::size_t i) {
        return base::operator[](i - 1);
    }

    /**
     * \brief Prints info about hyphenated word.
     * \todo Refactor and make const after removal of redundant growing arrays.
     */
    void print() {
        std::cout << "hyphenated_word";
        for (const auto& i = base::begin(); i != base::end(); ++i)
            std::cout << " " << *i;

        std::cout << std::endl << "dots";
        for (std::size_t i = 0; i <= this->size(); ++i)
            std::cout << " " << type[i];
        std::cout << std::endl << "dotw";
        for (std::size_t i = 0; i <= this->size(); ++i)
            std::cout << " " << weight[i];
        std::cout << std::endl << "hval";
        for (std::size_t i = 0; i <= this->size(); ++i)
            std::cout << " " << level[i];
        std::cout << std::endl << "no_m";
        for (std::size_t i = 0; i <= this->size(); ++i)
            if (is_knocked_out[i])
                std::cout << " t";
            else
                std::cout << " f";
        std::cout << std::endl;
    }

};

} // namespace ptl
