//
// Created by maria on 08.03.17.
//
#ifndef ANY_SOO_ANY_SOO_H
#define ANY_SOO_ANY_SOO_H
#include <bits/stdc++.h>
#include <utility>
#include <cstdio>
#include <type_traits>
#include <memory>
#include "memory_operations.h"

using namespace memory_operations;

class any {
    static const size_t BUFFER_SIZE = 16;

    enum object_size {
        SMALL,
        BIG,
        NONE
    } size;

    union {
        std::aligned_storage<BUFFER_SIZE, BUFFER_SIZE>::type buffer;
        void* heap_ptr;
    };

    using destructor_t = void (*) (void*);
    destructor_t destructor;
    using copy_t = void(*)(void*, void const*);
    copy_t copy;
    using move_t = void(*)(void*, void*);
    move_t move;
    using alloc_t =void*(*)();
    alloc_t allocator;
    using type_t = const std::type_info&(*)();
    type_t m_type;

    void* get_ptr() const {
        if (size == SMALL) {
            return (void*)&buffer;
        } else if (size== BIG) {
            return heap_ptr;
        } else {
            return nullptr;
        }
    }

public:

    void clear() {
        if (size == SMALL) {
            destructor(&buffer);
        } else if (size == BIG) {
            destructor(heap_ptr);
        }
        size = NONE;
    }

    template <typename T>
    using to_heap = typename std::integral_constant<bool, (sizeof(T) > BUFFER_SIZE)>;

private:
    template <typename T, typename B = typename to_heap<T>::type>
    void constructor(T object)
    {
        if (!B::value) {
            size = SMALL;
            new(&buffer) T(object);
            destructor = buffer_destructor<T>;
        } else {
            size = BIG;
            heap_ptr = new T(object);
            destructor = heap_destructor<T>;
        }
        copy = base_copy<T>;
        move = base_move<T>;
        allocator = base_alloc<T>;
        m_type = base_type<T>;
    }

public:

    template<typename T>
    any(T&& object,
        typename std::enable_if<!std::is_same<any&, T>::value>::type* = 0,
        typename std::enable_if<!std::is_const<T>::value>::type* = 0)
    {
        constructor<typename std::decay<T>::type >(std::forward<T>(object));
    }


    any(): size(NONE),
           buffer({}),
           copy([](void*, void const*){}),
           move([](void*, void*){}),
           allocator([]() -> void* { return nullptr; }),
           destructor([](void*){}),
           m_type(base_type<void>)
    {}

    any(any const &other):size(other.size),
                          copy(other.copy),
                          move(other.move),
                          allocator(other.allocator),
                          destructor(other.destructor),
                          m_type(other.m_type)
    {
        if (size == SMALL) {
            copy(&buffer, &other.buffer);
        } else if (size == BIG) {
            void* ptr = allocator();
            copy(ptr, other.heap_ptr);
            heap_ptr = ptr;
        }
    }

    any(any&& other):size(std::move(other.size)),
                     copy(std::move(other.copy)),
                     move(std::move(other.move)),
                     allocator(std::move(other.allocator)),
                     destructor(std::move(other.destructor)),
                     m_type(std::move(other.m_type))
    {
        if (size == SMALL) {
            move(&buffer, &other.buffer);
        } else if (size == BIG) {
            heap_ptr = other.heap_ptr;
            other.heap_ptr = nullptr;
            other.size = NONE;
        }
    }

    ~any() {
        clear();
    }

    any & swap(any& other) {
        std::swap(this->size, other.size);
        std::swap(this->buffer, other.buffer);
        std::swap(this->destructor, other.destructor);
        std::swap(this->copy, other.copy);
        std::swap(this->move, other.move);
        std::swap(this->allocator, other.allocator);
        std::swap(this->m_type, other.m_type);
        return *this;
    }

    bool empty() const {
        return (size == NONE);
    }

    const std::type_info& type() const{
        return this->m_type();
    }

    any& operator=(any const& other) {
        any(other).swap(*this);
        return *this;
    }

    any& operator=(any&& other) {
        any(std::move(other)).swap(*this);
    }

    template<typename T>
    any& operator=(T&& other) {
        any(std::forward<T>(other)).swap(*this);
        return *this;
    }

    template <typename T>
    friend T any_cast(const any& op);

    template <typename T>
    friend T any_cast(any& op);

    template <typename T>
    friend T any_cast(any&& op);

    template <typename T>
    friend T const * any_cast(any const *op);

    template <typename T>
    friend T * any_cast(any *op);
};

template <typename T>
T any_cast(const any& op) {
    void* r = op.get_ptr();
    return *(std::add_const_t<std::remove_reference_t<T>>*)r;
}

template <typename T>
T any_cast (any& op) {
    void* r = op.get_ptr();
    return *(std::remove_reference_t<T> *)r;
}

template <typename T>
T any_cast (any&& op) {
    void* r = op.get_ptr();
    return *(std::remove_reference_t<T> *)r;
}

template <typename T>
T const * any_cast(any const *op) {
    if (op == nullptr || op->type() != typeid(T)) {
        return nullptr;
    }
    const T *r = (T const *)op->get_ptr();
    return r;
}

template <typename T>
T * any_cast(any *op) {
    if (op == nullptr || op->type() != typeid(T)) {
        return nullptr;
    }

    T *r = (T*)op->get_ptr();
    return r;
}

#endif //ANY_SOO_ANY_SOO_H
