//
// Created by maria on 30.10.16.
//

#ifndef CHECKED_CHECKED_H
#define CHECKED_CHECKED_H

#include <iostream>
#include <limits>

struct check_exception {
public:
    check_exception(std::string const& err) :
            error(err)
    {}
    std::string const& what() {
        return this->error;
    }
private:
    std::string error;
};

struct unsigned_tag {};
struct signed_tag {};

template <typename T>
struct switch_tag;

template <typename T, bool>
struct checked_tag;

template <typename T>
struct checked_tag<T, true> {
    typedef signed_tag tag;
};

template <typename T>
struct checked_tag<T, false> {
    typedef unsigned_tag tag;
};

template <typename T>
struct switch_tag {
    typedef typename checked_tag<T, std::numeric_limits<T>::is_signed>::tag tag;
};


template <typename V>
struct checked {
public:
    checked() :
            value(V())
    {}
    checked(V value) :
            value(value)
    {}
    checked<V>& operator=(checked<V> const& rhs) {
        this->value = rhs.value;
        return *this;
    }

    template <typename T>
    friend checked<T> operator+(checked<T> const&, checked<T> const&);
    template <typename T>
    friend checked<T> operator-(checked<T> const&, checked<T> const&);
    template <typename T>
    friend checked<T> operator*(checked<T> const&, checked<T> const&);
    template <typename T>
    friend checked<T> operator/(checked<T> const&, checked<T> const&);

    template <typename T>
    friend checked<T> add(checked<T> const&, checked<T> const&, signed_tag);
    template <typename T>
    friend checked<T> add(checked<T> const&, checked<T> const&, unsigned_tag);
    template <typename T>
    friend checked<T> sub(checked<T> const&, checked<T> const&, signed_tag);
    template <typename T>
    friend checked<T> sub(checked<T> const&, checked<T> const&, unsigned_tag);
    template <typename T>
    friend checked<T> mul(checked<T> const&, checked<T> const&, signed_tag);
    template <typename T>
    friend checked<T> mul(checked<T> const&, checked<T> const&, unsigned_tag);
    template <typename T>
    friend checked<T> divide(checked<T> const&, checked<T> const&, signed_tag);
    template <typename T>
    friend checked<T> divide(checked<T> const&, checked<T> const&, unsigned_tag);

    template <typename T>
    checked<T> unary_minus(signed_tag) {
        if (this->value == std::numeric_limits<T>::min()) {
            throw check_exception("overflow at unary minus");
        }
        return checked<T>(-this->value);
    }
    template <typename T>
    checked<T> unary_minus(unsigned_tag) {
        if (this->value == 0) {
            throw check_exception("called for zero");
        }
        checked(0);
    }
    template <typename T>
    checked<T> operator-() {
        return unary_minus(typename switch_tag<T>::tag());
    }
private:
    V value;
};


template <typename T>
checked<T> add(checked<T> const & a, checked<T> const & b, signed_tag) {
    if ((b.value > 0 && a.value > std::numeric_limits<T>::max() - b.value) ||
        (b.value < 0 && a.value < std::numeric_limits<T>::min() - b.value)) {
        throw check_exception("overflow at signed add");
    }
    return checked<T>(a.value + b.value);
}

template <typename T>
checked<T> add(checked<T> const & a, checked<T> const & b, unsigned_tag) {
    if (a.value > std::numeric_limits<T>::max() - b.value) {
        throw check_exception("overflow at unsigned add");
    }
    return checked<T>(a.value + b.value);
}

template <typename T>
checked<T> sub(checked<T> const & a, checked<T> const & b, signed_tag) {
    if ((b.value > 0 && a.value < std::numeric_limits<T>::min() + b.value) ||
        (b.value < 0 && a.value > std::numeric_limits<T>::max() + b.value)) {
        throw check_exception("overflow at signed sub");
    }
    return checked<T>(a.value - b.value);
}

template<typename T>
checked<T> sub(checked<T> const & a, checked<T> const & b, unsigned_tag) {
    if (a.value < std::numeric_limits<T>::min() + b.value) {
        throw check_exception("overflow at unsigned sub");
    }
    return checked<T>(a.value - b.value);
}

template <typename T>
checked<T> mul(checked<T> const & a, checked<T> const & b, signed_tag) {
    if ((a.value > 0 && b.value > 0 && a.value > std::numeric_limits<T>::max() / b.value) ||
        (a.value > 0 && b.value < 0 && b.value < std::numeric_limits<T>::min() / a.value) ||
        (a.value < 0 && b.value > 0 && a.value < std::numeric_limits<T>::min() / b.value) ||
        (a.value < 0 && b.value < 0 && a.value != 0 && b.value < std::numeric_limits<T>::max() / a.value)) {
        throw check_exception("overflow at signed mul");
    }
    return checked<T>(a.value * b.value);
}

template <typename T>
checked<T> mul(checked<T> const & a, checked<T> const & b, unsigned_tag) {
    if (a.value > std::numeric_limits<T>::max() / b.value) {
        throw check_exception("overflow at unsigned mul");
    }
    return checked<T>(a.value * b.value);
}

template <typename T>
checked<T> divide(checked<T> const & a, checked<T> const & b, signed_tag) {
    if (b.value == 0 || (a.value == std::numeric_limits<T>::min() && b.value == -1)) {
        throw check_exception("overflow at signed div");
    }
    return checked<T>(a.value / b.value);
}

template <typename T>
checked<T> divide(checked<T> const & a, checked<T> const & b, unsigned_tag) {
    return checked<T>(a.value / b.value);
}



template <typename T>
checked<T> operator+(checked<T> const & a, checked<T> const & b) {
    return add(a, b, typename switch_tag<T>::tag());
}

template <typename T>
checked<T> operator-(checked<T> const & a, checked<T> const & b) {
    return sub(a, b, typename switch_tag<T>::tag());
}

template <typename T>
checked<T> operator*(checked<T> const & a, checked<T> const & b) {
    return mul(a, b, typename switch_tag<T>::tag());
}

template <typename T>
checked<T> operator/(checked<T> const & a, checked<T> const & b) {
    return divide(a, b, typename switch_tag<T>::tag());
}

#endif //CHECKED_CHECKED_H
