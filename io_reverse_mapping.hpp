#pragma once

#include <map>
#include <vector>

namespace ptl {

template <class Texternal, class Tinternal>
class io_reverse_mapping {
protected:
    std::map<Tinternal, std::vector<Texternal>> mapping;

public:
    void insert(const Tinternal& i, const std::vector<Texternal>& v) {
        mapping[i] = v;
    }

    void add_to_string(const Tinternal& i, std::basic_string<Texternal>& s) {
        auto it = mapping.find(i);
        s.insert(s.end(), it->second.begin(), it->second.end());
    }

};
    
} // namespace ptl