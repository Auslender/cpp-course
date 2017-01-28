//
// Created by maria on 12.10.16.
//

#ifndef PERSISTENT_SET_PERSISTENT_SET_H
#define PERSISTENT_SET_PERSISTENT_SET_H

#include <sys/param.h>
#include <cstdint>
#include <utility>
#include <cstdlib>
#include <stack>
#include <memory>

struct persistent_set
{
    // Вы можете определить этот тайпдеф по вашему усмотрению.
    typedef int32_t value_type;
    // Bidirectional iterator.
    struct iterator;

    // Создает пустой persistent_set.
    persistent_set();

    // Создает копию указанного persistent_set-а.
    persistent_set(persistent_set const&);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    persistent_set& operator=(persistent_set const& rhs);

    // Деструктор. Вызывается при удалении объектов persistent_set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого persistent_set
    // (включая итераторы ссылающиеся на элементы следующие за последними).
    ~persistent_set();

    // Поиск элемента.
    // Возвращает итератор на элемент найденный элемент, либо end().
    iterator find(value_type);

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производится, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производится вставка, возвращается итератор на созданный
    //    элемент и true.
    // Если вставка произведена, инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    std::pair<iterator, bool> insert(value_type);

    // Удаление элемента.
    // Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    void erase(iterator);

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin() const;
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end() const;
private:
    struct node;
    typedef std::shared_ptr<node> sp;

    struct node {
        node() {};
        node(value_type val, sp l, sp r) :
                value(val),
                left(l),
                right(r)
        {};
        sp left = nullptr;
        sp right = nullptr;
        value_type value;
    };
    sp root;
    sp fake_node = std::shared_ptr<node>(new node());
    sp insert(sp inserted, sp root, std::stack<sp> path);
    sp erase(sp erased, sp root, std::stack<sp> path);
    sp rec_del_min(sp root);

    iterator find_min(sp root, std::stack<sp> path) const;
    iterator find_max(sp root, std::stack<sp> path) const;

    sp find_min(sp root);
};

struct persistent_set::iterator
{
    // Элемент на который сейчас ссылается итератор.
    // Разыменование итератора end() неопределено.
    // Разыменование невалидного итератора неопределено.
    value_type const& operator*() const;

    // Переход к элементу со следующим по величине ключом.
    // Инкремент итератора end() неопределен.
    // Инкремент невалидного итератора неопределен.
    iterator& operator++();
    iterator operator++(int);

    // Переход к элементу с предыдущим по величине ключом.
    // Декремент итератора begin() неопределен.
    // Декремент невалидного итератора неопределен.
    iterator& operator--();
    iterator operator--(int);

private:
    //конструктор итератора от ссылки на persistent_set, и stack с путем от данной вершины до корня
    iterator(const persistent_set& ps, sp n, std::stack<sp> p) :
            ps(ps),
            n(n),
            path(p)
    {};
    const persistent_set& ps;
    sp n;
    //храним путь от корня до вершины, на которую указывает итератор
    std::stack<sp> path;
    sp get_root();
    friend class persistent_set;
    friend bool operator==(iterator first, iterator second);
    friend bool operator!=(iterator first, iterator second);
};

// Сравнение. Итераторы считаются эквивалентными если одни ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
bool operator==(persistent_set::iterator, persistent_set::iterator);
bool operator!=(persistent_set::iterator, persistent_set::iterator);
#endif //PERSISTENT_SET_PERSISTENT_SET_H