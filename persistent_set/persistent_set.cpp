//
// Created by maria on 12.10.16.
//
#include "persistent_set.h"

persistent_set::persistent_set() :
        root(nullptr)
{}

persistent_set::persistent_set(persistent_set const & other) :
        root(other.root)
{}

persistent_set& persistent_set::operator=(persistent_set const &rhs) {
    root = rhs.root;
    return *this;
}

persistent_set::~persistent_set() {

}

persistent_set::iterator persistent_set::find(value_type value) {
    std::stack<sp> path;
    sp curr = root;
    while (curr != nullptr && curr->value != value) {
        path.push(curr);
        if (value < curr->value) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if (curr == nullptr) {
        return this->end();
    }
    return iterator(*this, curr, path);
}

std::pair<persistent_set::iterator, bool> persistent_set::insert(value_type value) {
    if (root == nullptr) {
        root = std::shared_ptr<node>(new node(value, nullptr, nullptr));
        std::stack<sp> path;
        return std::make_pair(persistent_set::iterator(*this, root, path), true);
    }
    iterator place = find(value);
    //если нашли
    if (place != this->end()) {
        return std::make_pair(place, false);
    }
    sp inserted = std::shared_ptr<node>(new node(value, nullptr, nullptr));
    //здесь инвалидируется старый root
    // рекурсивно идем от корня до места вставки, создавая новые вершины
    std::stack<sp> path;
    root = insert(inserted, root, path);
    return std::make_pair(persistent_set::iterator(*this, inserted, path), true);
}

persistent_set::sp persistent_set::insert(sp inserted, sp root, std::stack<sp> path) {
    if (inserted->value > root->value) {
        if (root->right == nullptr) {
            return std::shared_ptr<node>(new node(root->value, root->left, inserted));
        }
        path.push(root);
        return std::shared_ptr<node>(new node(root->value, root->left, insert(inserted, root->right, path)));
    }
    if (inserted->value < root->value) {
        if (root->left == nullptr) {
            return std::shared_ptr<node>(new node(root->value, inserted, root->right));
        }
        path.push(root);
        return std::shared_ptr<node>(new node(root->value, insert(inserted, root->left, path), root->right));
    }
}

persistent_set::value_type const& persistent_set::iterator::operator*() const {
    return this->n->value;
}

persistent_set::iterator persistent_set::find_min(sp root, std::stack<sp> path) const{
    if(root->left == nullptr) {
        return iterator(*this, root, path);
    }
    path.push(root);
    return find_min(root->left, path);
}

persistent_set::iterator persistent_set::find_max(sp root, std::stack<sp> path) const {
    if (root == nullptr || root->right == nullptr) {
        return iterator(*this, root, path);
    }
    path.push(root);
    return find_max(root->right, path);
}

persistent_set::iterator& persistent_set::iterator::operator++() {
    if (n->right != nullptr) {
        iterator place = this->ps.find_min(n->right, path);
        n = place.n;
        path = place.path;
        return *this;
    }
    std::stack<sp> path;
    if (*this == this->ps.find_max(ps.root, path)) {
        this->n = ps.fake_node;
        return *this;
    }
    sp x = n;
    sp y = this->path.top();
    this->path.pop();
    while(!this->path.empty() && x == y->right) {
        x = y;
        y = this->path.top();
        this->path.pop();
    }
    this->n = y;
    return *this;
}

persistent_set::iterator& persistent_set::iterator::operator--() {
    if (n->left != nullptr) {
        iterator place = this->ps.find_max(n->left, path);
        n = place.n;
        path = place.path;
        return *this;
    }
    if (*this == ps.end()) {
        iterator place = this->ps.find_max(ps.root, path);
        n = place.n;
        path = place.path;
        return *this;
    }
    sp x = n;
    sp y = path.top();
    this->path.pop();
    while (y != nullptr && x == y->left) {
        x = y;
        y = path.top();
        path.pop();
    }
    n = y;
    return *this;
}

persistent_set::iterator persistent_set::iterator::operator++(int) {
    iterator ret = *this;
    ++(*this);
    return ret;
}

persistent_set::iterator persistent_set::iterator::operator--(int) {
    iterator ret = *this;
    --(*this);
    return ret;
}

persistent_set::iterator persistent_set::begin() const {
    if (root == nullptr) {
        return this->end();
    }
    std::stack<sp> path;
    return find_min(root, path);
}

persistent_set::iterator persistent_set::end() const {
    std::stack<sp> path;
    return iterator(*this, fake_node, find_max(root, path).path);
}

void persistent_set::erase(iterator erased) {
    std::stack<sp> path;
    root = erase(erased.n, root, path);
}

persistent_set::sp persistent_set::erase(sp erased, sp root, std::stack<sp> path) {
    if (erased->value > root->value) {
        path.push(root);
        return std::shared_ptr<node>(new node(root->value, root->left, erase(erased, root->right, path)));
    }
    if (erased->value < root->value) {
        path.push(root);
        return std::shared_ptr<node>(new node(root->value, erase(erased, root->left, path), root->right));
    }
    if (root->right != nullptr) {
        return std::shared_ptr<node>(new node(find_min(root->right)->value, root->left, rec_del_min(root->right)));
    }
    if (root->right == nullptr) {
        if (root->left == nullptr) {
            return nullptr;
        }
        return std::shared_ptr<node>(new node(root->left->value, root->left->left, root->left->right));
    }
}

persistent_set::sp persistent_set::rec_del_min(sp root) {
    if (root->left == nullptr && root->right == nullptr) {
        return nullptr;
    }
    if (root->left == nullptr && root->right != nullptr) {
        return std::shared_ptr<node>(new node(root->right->value, root->right->left, root->right->right));
    }
    return std::shared_ptr<node>(new node(root->value, rec_del_min(root->left), root->right));
}


persistent_set::sp persistent_set::find_min(sp root) {
    sp curr = root;
    while (curr->left != nullptr) {
        curr = curr->left;
    }
    return curr;
}

bool operator==(persistent_set::iterator first, persistent_set::iterator second) {
    return (first.get_root() == second.get_root() && first.n == second.n);
}

bool operator!=(persistent_set::iterator first, persistent_set::iterator second) {
    return !(first == second);
}

persistent_set::sp persistent_set::iterator::get_root() {
    return this->ps.root;
}


