#include <concepts>
#include <iostream>
#include <iterator>
#include <vector>

// 13.6 Consepts
template<typename T>
concept InputIterator = requires(T a) {
    {++a} -> std::same_as<T&>;
    typename std::iterator_traits<T>::value_type;
    {*a} -> std::convertible_to<typename std::iterator_traits<T>::value_type>;
};


template<InputIterator It, std::predicate<decltype(*std::declval<It>())> Pred>
requires InputIterator<It>
&& requires(Pred p, It it) { p(*it); }
It my_find_if(It beg, It end, Pred p) {
  for (auto it = beg; it != end; ++it) {
    if (p(*it)) {
      return it;
    }
  }
  return end;
}

int main() {
  std::vector<int> v(10);

  my_find_if(v.begin(), v.end(),);
}