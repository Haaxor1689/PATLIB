#pragma once

#include <map>
#include <set>
#include <vector>

namespace ptl {

template <class External>
class translation_service {
    typedef unsigned Internal;

    External* externals;
    std::map<External, Internal> internals;
    Internal last_used_internal;

public:
    translation_service(const std::set<External>& ext_set) {
        internals.clear();
        last_used_internal = 0;
        externals = new External[ext_set.size() + 1];

        for (auto i = ext_set.begin();
             i != ext_set.end(); i++) {
            last_used_internal++;
            externals[last_used_internal] = *i;
            internals.insert(std::make_pair(*i, last_used_internal));
        }
    }

    virtual ~translation_service() {
        delete[]externals;
    }

    virtual Internal get_last_used_internal() const {
        return last_used_internal;
    }

    virtual Internal to_internal(const External& e) const {
        return internals.find(e)->second;
    }

    virtual std::vector<Internal> to_internal(const std::vector<External>& ve) const {
        std::vector<Internal> vi;
        for (auto i = ve.begin(); i != ve.end(); ++i) {
            vi.push_back(to_internal(*i));
        }
        return vi;
    }

    virtual External to_external(const Internal& i) const {
        return externals[i];
    }

    virtual std::vector<External> to_external(const std::vector<Internal>& vi) const {
        std::vector<External> ve;
        for (auto i = vi.begin(); i != vi.end(); ++i) {
            ve.push_back(to_external(*i));
        }
        return ve;
    }

};

} // namespace ptl