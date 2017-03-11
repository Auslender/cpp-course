//
// Created by maria on 08.03.17.
//

#ifndef ANY_SOO_MEMORY_OPERATIONS_H
#define ANY_SOO_MEMORY_OPERATIONS_H
namespace memory_operations {

    template<typename D>
    static void buffer_destructor (void *buffer_ptr) {
        ((D *) buffer_ptr)->~D();
    };

    template<typename D>
    static void heap_destructor (void *heap_ptr) {
        delete ((D *) heap_ptr);
    };

    template<typename C>
    static void base_copy(void* to, void const* from) {
        new(to) C(*(C const*) from);
    }

    template<typename A>
    static void* base_alloc() {
        return new typename std::aligned_storage<sizeof(A), alignof(A)>::type;
    }

    template <typename M>
    static void base_move(void* to, void* from) {
        new(to) M(std::move(*(M*)from));
    }

    template <typename T>
    static const std::type_info& base_type() {
        return typeid(T);
    }
}

#endif //ANY_SOO_MEMORY_OPERATIONS_H
