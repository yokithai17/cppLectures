#include <iostream>
#include <algorithm>
#include <vector>
// 8.5 Output iterators and stream iterators.

template <typename Container>
class back_insert_iterator {
    Container& container;

public:
    back_insert_iterator(Container& cont)
    : container(cont) {}

    back_insert_iterator&
    operator=(const typename Container::valut_type& val) {
        Container.push_back(val);
        return *this;
    }

    back_insert_iterator& operator++() {
        return *this;
    }
    
    back_insert_iterator operator++(int) {
        return *this;
    }

    back_insert_iterator& operator*() {
        return *this;
    }
};

template <typename Cont>
back_insert_iterator<Cont> back_inserter(Cont& cont) {
    return {cont};
}


int main() {
    int a[10] = {1, 2, 3, 4, 5};
    std::vector<int> v;

    std::copy(a, a + 10, v.begin());
}