#pragma once

#include <ostream>

namespace ptl {

/**
 * \brief Type of possible hyphenations.
 */
enum class hyphenation_type {
    /**
     * \brief No hyphenation.
     */
    none,

    /**
     * \brief Erroneous hyphenation.
     */
    wrong,

    /**
     * \brief Correct hyphenation.
     */
    correct,

    /**
     * \brief Correct hyphenation found in past.
     */
    past
};

inline std::ostream& operator<<(std::ostream& os, hyphenation_type value) {
    switch (value) {
    case hyphenation_type::none: return os << "hyphenation_type::none";
    case hyphenation_type::wrong: return os << "hyphenation_type::wrong";
    case hyphenation_type::correct: return os << "hyphenation_type::correct";
    case hyphenation_type::past: return os << "hyphenation_type::past";
    default: return os << "hyphenation_type::NOT_VALID_ENUM_CASE";
    }
}

}
