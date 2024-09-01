#include <iostream>

template <typename InputIterator>
void find_most_often_number(InputIterator begin, InputIterator end) {
    // auto x = *begin Почему это плохо, к примеру если бы у нас был
    // std::vector<bool> который вернул бы нам bit reference
    typename std::iterator_traits<InputIterator>::value_type x = *begin;
}