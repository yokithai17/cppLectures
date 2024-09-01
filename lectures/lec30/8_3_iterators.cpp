#include <iostream>
#include <iterator>
#include <vector>

template <typename Iterator>
typename std::iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator) {
    if constexpr (std::is_base_of_v<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag
        >) {
        return last - first;
    }

    typename std::iterator_traits<Iterator>::difference_type i = 0;
    for (; first != last; ++i, ++first);
    return i;
}