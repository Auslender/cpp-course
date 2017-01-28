#include "my_container.h"
#include <assert.h>

void my_container::detach() {
    if (!long_buffer.unique()) {
        long_buffer = std::make_shared<std::vector<int64_t>>(*long_buffer);
    }
}

my_container::my_container():
        small(1), small_buffer(0)
{}

my_container::~my_container() {}

my_container::my_container(my_container const &other) {
    this->small = other.small;
    if (other.small) {
        this->small_buffer = other.small_buffer;
    } else {
        this->long_buffer = other.long_buffer;
    }
}

my_container& my_container::operator=(my_container const &other) {
    this->small = other.small;
    if (other.small) {
        small_buffer = other.small_buffer;
    } else {
        long_buffer = other.long_buffer;
    }
    return *this;
}

int64_t const& my_container::back() const {
    if (this->small) {
        return small_buffer;
    } else {
        return long_buffer->back();
    }
}

size_t my_container::size() const {
    if (this->small) {
        return 1;
    }
    return this->long_buffer->size();
}

void my_container::push_back(const int64_t& a) {
    if (this->small) {
        small = 0;
        int64_t sb = small_buffer;
        long_buffer = std::make_shared<std::vector<int64_t> >();
        long_buffer->push_back(sb);
        long_buffer->push_back(a);
        return;
    }
    this->detach();
    long_buffer->push_back(a);
}

void my_container::pop_back() {

    this->detach();
    long_buffer->pop_back();
}

void my_container::resize(size_t sz) {
    if (small && sz > 1) {
        small = 0;
        int64_t sb = small_buffer;
        long_buffer = std::make_shared<std::vector<int64_t> >(sz);
        (*long_buffer)[0] = sb;
        return;
    }
    if (small && sz == 1) {
        return;
    }
    this->detach();
    long_buffer->resize(sz);
}

int64_t& my_container::operator[](size_t i) {
    if (small) {
        assert(i == 0);
        return small_buffer;
    }
    this->detach();
    return (*long_buffer)[i];
}

int64_t const& my_container::operator[](size_t i) const {
    if (small) {
        assert(i == 0);
        return small_buffer;
    }
    return (*long_buffer)[i];
}
