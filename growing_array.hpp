#pragma once

#include <iostream>
#include <vector>
#include "exception.hpp"

namespace ptl {

/**
 * \brief We need an automatically growing array of given type.
 *        We want to add to the array mostly, deallocating is done at the end of work.
 *        The growing array is addressed by linear address and it grows when addressing previously unaccessed member.
 *        Addressing is done with overloaded operator[].
 * \tparam T The type of the elements.
 */
template <class T>
class growing_array : public std::vector<T> {
    using base = std::vector<T>;
    
    /**
     * \brief Default value used when resizing the array.
     */
    const T default_value;

public:
    /**
     * \brief Constructor
     * \param default_value  Default value used for constructing non existing elements.
     */
    growing_array(const T& default_value) : base(), default_value(default_value) {}
    
    /**
     * \brief Index access operator. Constructs new elements if index is bigger than size.
     * \param index Index of requested element.
     * \return Reference to element on given index.
     */
    T& operator[](std::size_t index) {
        if (index >= base::size()) {
            base::resize(index, default_value);
        }
        return base::operator[](index);
    }

    /**
     * \brief Prints statistics about array to standard output stream.
     */
    void print_statistics() const {
        std::cout << "  No. of members used       " << std::vector<T>::size() << std::endl;
        std::cout << "  No. of members reserved   " << std::vector<T>::capacity() << std::endl;
        std::cout << "  Member size (bytes)       " << sizeof(T) << std::endl;
    }
};

} // namespace ptl
