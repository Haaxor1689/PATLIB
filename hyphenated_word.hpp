# pragma once

#include <iostream>
#include <vector>
#include "growing_array.hpp"

namespace ptl {

enum class hyphenation_type {
    none,
    wrong,
    correct,
    past
};

inline std::ostream& operator<<(std::ostream& os, hyphenation_type type) {
    switch (type) {
    case hyphenation_type::none: return os << "hyphenation_type::none";
    case hyphenation_type::wrong: return os << "hyphenation_type::wrong";
    case hyphenation_type::correct: return os << "hyphenation_type::correct";
    case hyphenation_type::past: return os << "hyphenation_type::past";
    default: return os << "hyphenation_type::NOT_VALID_ENUM_CASE";
    }
}

    /**
 * \brief Class for holding info about hyphenated word.
 * \todo Remove 4 redundant growing arrays.
 * \tparam Tin_alph Element type.
 * \tparam Twt_type Weight type.
 * \tparam Tval_type Level type.
 */
template <class Tin_alph, class Twt_type, class Tval_type>
class hyphenated_word : public std::vector<Tin_alph> {
    using base = std::vector<Tin_alph>;
    
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
