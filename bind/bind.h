#ifndef BIND_BIND_H
#define BIND_BIND_H
#include <tuple>
#include <functional>
#include <type_traits>

template <size_t N>
struct placeholder{};
placeholder<0> _1;
placeholder<1> _2;
placeholder<2> _3;
placeholder<3> _4;
placeholder<4> _5;
placeholder<5> _6;
placeholder<6> _7;
placeholder<7> _8;
placeholder<8> _9;
placeholder<9> _10;


template<typename F, typename ... Bound_Args>
struct binder
{
private:
    template <typename F_, typename ... Bound_Args_>
    friend binder<F_, Bound_Args_ ...> bind(F_&&, Bound_Args_&& ...);

    typedef typename std::decay_t<F> F_t;
    typedef typename std::tuple<std::decay_t<Bound_Args> ...> tuple_t;

    F_t f;
    tuple_t tuple;

    binder(F&& f, Bound_Args&& ... bound_args) :
            f(std::forward<F>(f)),
            tuple(std::forward<Bound_Args>(bound_args) ...)
    {}

    template <size_t ...>
    struct seq_of_indices {};

    template <size_t N, size_t ... S>
    struct unpack_tuple : unpack_tuple<N - 1, N - 1, S ...> {};

    template <size_t ... S>
    struct unpack_tuple<0, S ...> {
        typedef seq_of_indices<S ...> type;
    };

    template <typename Bound_Arg, typename ... Const_Args>
    auto&& get_arg (Bound_Arg& bound_arg, Const_Args& ... const_args) {
        return bound_arg;
    };


    template <size_t N, typename ... Const_Args>
    auto&& get_arg (placeholder<N>&, Const_Args& ... const_args) {
        return std::get<N>(std::forward_as_tuple(const_args ...));
    };


    template <typename Rec_Call, typename ... Rec_Bound_Args, typename ... Const_Args>
    auto get_arg(binder<Rec_Call, Rec_Bound_Args ...>& rec_binder, Const_Args& ... const_args) {
        return rec_binder(const_args ...);
    };


    template <size_t ... N, typename ... Const_Args>
    auto call_function(const seq_of_indices<N ...>&, Const_Args&& ... const_args) {
        return f(get_arg(std::get<N>(tuple), const_args ...) ... );
    };

public:
    template <typename ... Const_Args>
    auto operator() (Const_Args&& ... const_args) {
        return call_function(typename unpack_tuple<
                                     std::tuple_size<
                                             std::tuple<std::decay_t<Bound_Args> ...> >::value>::type(),
                             std::forward<Const_Args>(const_args) ...);
    }
};

template <typename F, typename ... Bound_Args>
binder<F, Bound_Args ...> bind(F&& f, Bound_Args&& ... bound_args) {
    return binder<F, Bound_Args ...>(std::forward<F>(f), std::forward<Bound_Args>(bound_args) ...);
};

#endif //BIND_BIND_H
