#pragma once

namespace ptl {
enum class char_class {
    space,
    digit,
    hyf,
    letter,
    escape,
    invalid
};

inline bool operator==(Tcount_good lhs, char_class rhs) { return lhs == Tcount_good(rhs); }
inline bool operator!=(Tcount_good lhs, char_class rhs) { return lhs != rhs; }

} // namespace ptl
