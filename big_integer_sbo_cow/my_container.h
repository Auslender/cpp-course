#ifndef BIGINT_MY_CONTAINER_H
#define BIGINT_MY_CONTAINER_H

#include <stdio.h>
#include <memory>
#include <vector>

/*struct COWptr
{
    std::shared_ptr<std::vector<int64_t> > shared_p;
    COWptr();
    COWptr(COWptr const& other);

    //void detach();
};*/

struct my_container
{
    my_container();
    ~my_container();
    my_container(my_container const& other);
    my_container& operator=(my_container const& other);

    int64_t const& back() const;

    void resize(size_t s);
    size_t size() const;

    int64_t const& operator[](size_t i) const;
    int64_t& operator[](size_t i);

    void pop_back();
    void push_back(const int64_t& a);

private:
    bool small;
    std::shared_ptr<std::vector<int64_t> > long_buffer;
    int64_t small_buffer;
    void detach();
};

#endif //BIGINT_MY_CONTAINER_H