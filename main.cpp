#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>
#include <type_traits>

// эта хрень нужна для того, чтобы юзать range
template <typename A>
using container_compatible_range = std::enable_if_t<
        std::is_same_v<decltype(std::declval<A>().begin()), decltype(std::declval<A>().end())>
>;

using std::cin;
using std::cout;
using std::swap;

// Узел дерева.
template<typename T>
class Node {
public:
    T val;
    char Color{};
    Node *left;
    Node *right;
    Node *parent;

    explicit Node(char Color) {
        this->val = val;
        this->Color = Color;
        this->left = nullptr;
        this->right = nullptr;
        this->parent = nullptr;
    }

    Node(T val, char Color) {
        this->val = val;
        this->Color = Color;
        this->left = nullptr;
        this->right = nullptr;
        this->parent = nullptr;
    }
};

// Объект хранящий актуальную информацию о максимальном и минимальном значение в дереве.
template<typename T>
class Control{
private:
    Node<T> *maxNode{nullptr};
    Node<T> *minNode{nullptr};
public:
    ~Control() = default;

    Control() = default;

    void addition(Node<T> *curr){
        if(maxNode == nullptr && minNode == nullptr) minNode = maxNode = curr;
        else if(maxNode->val < curr->val) maxNode = curr;
        else if(minNode->val > curr->val) minNode = curr;
        else return;
    }

    void deletion(Node<T> *curr){
        if(curr == nullptr) return;

        if(maxNode->val == curr->val) {
            if(maxNode->left != nullptr) maxNode = maxNode->left;
            else maxNode = maxNode->parent;
        }
        else if(minNode->val == curr->val) {
            if(minNode->right != nullptr) minNode = minNode->right;
            else minNode = minNode->parent;
        }
        else return;
    }

    void clear(){
        maxNode = nullptr;
        minNode = nullptr;
    }

    [[nodiscard]] Node<T> * getMaxNode() const{
        return maxNode;
    }

    [[nodiscard]] Node<T> * getMinNode() const{
        return minNode;
    }

};

// Итераторы.
// Хоть как я говорил ниже, но вы скорее всего читаете сверху вниз итераторы потерпели изменения, и теперь
// в реализации есть ненужные инструкции, но тк мне ща лень и вообще есть проблемы с большим приоритетом, я чистить их от лишнего кода не буду.=)

// Если на словах объяснить алгоритм работы итератора при инкременте и декременте, то он выглядит следующим образом.
// Первым делом проверяются адреса на соседние узлы. Если они действительны, то ставим false(там не были) и true(там были)
// потом проверяем условия становления текущего итератора итератором end(). Для операции декремента очередность наоборот
// тк при инкременте итератор становиться итератором end(), а при декременте он может им быть. Информация о крайних узлах берется из объекта Control.
// Далее алгоритм переходи к основной логике итерирования по дереву. Пример для обычного итератора, инкремент.
//  1. Если существует родитель, и правый ребенок идем вправо и сразу же влево до конца. Выводим.
//  2. Если существует только родитель, то проверяем каким ребенком является текущий узел.
//    2.1 Если левый ребенок, то выводим родителя.
//    2.2 Если правый, то в цикле идем по родительскому к узлу до тех пор, пока предыдущий родитель, для текущего ни будет левым ребенком. Выводим.
// Для операции декремента аналогично, только стороны меняются местами.
// Для реверс-итераторов все то же самое, но применение алгоритмов для операторов ++ и -- меняется относительно обычного итератора.
// Константные итераторы это оболочка над обычными итераторами. Там только изменения в разыменовывании итератора.
template<typename T>
class Iterator {
private:
    Node<T> *id_node{};
    const Control<T>* control{};
    bool lef_t = false;
    bool righ_t = false;
    bool paren_t = false;

    void init_position_pp() {
        lef_t = true;
        id_node->right ? righ_t = false : righ_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
    }

    void init_position_mm() {
        righ_t = true;
        id_node->left ? lef_t = false : lef_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
    }


public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;

    ~Iterator() = default;

    Iterator() = default;

    explicit Iterator(Node<T> *curr, const Control<T> *info) {
        this->id_node = curr;
        this->control = info;
    }

    Iterator(const Iterator<T> &it) {
        this->id_node = it.id_node;
        this->control = it.control;
        this->lef_t = it.lef_t;
        this->righ_t = it.righ_t;
        this->paren_t = it.paren_t;
    }

    Iterator(Node<T> *curr, bool flag, const Control<T> *info) {
        control = info;
        if (flag) {
            if (curr->Color != 'W') {
                id_node = control->getMinNode();
            } else id_node = nullptr;
        } else id_node = nullptr;
    }

    Iterator<T> &operator++() {

        init_position_pp();

        if (id_node->val == control->getMaxNode()->val) {
            id_node = id_node->right;
            return *this;
        }
        if (!righ_t) {
            id_node = id_node->right;
            paren_t = false;
            lef_t = true;
            while (id_node->left) { id_node = id_node->left; }
            id_node->right ? righ_t = false : righ_t = true;
            return *this;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                tmp->right ? righ_t = false : righ_t = true;
                lef_t = true;
                id_node = tmp;
            } else {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->right == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->right ? righ_t = false : righ_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            }
            return *this;
        }
    }

    Iterator<T> operator++(int) { // NOLINT

        Iterator helper = *this;
        init_position_pp();

        if (id_node->val == control->getMaxNode()->val) {
            id_node = id_node->right;
            return helper;
        }
        if (!righ_t) {
            id_node = id_node->right;
            paren_t = false;
            lef_t = true;
            while (id_node->left) { id_node = id_node->left; }
            id_node->right ? righ_t = false : righ_t = true;
            return helper;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                tmp->right ? righ_t = false : righ_t = true;
                lef_t = true;
                id_node = tmp;
            } else {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->right == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->right ? righ_t = false : righ_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            }
            return helper;
        }
    }

    Iterator<T> &operator--() {
        if (!id_node) {
            id_node = control->getMaxNode();
            return *this;
        }
        init_position_mm();

        if (id_node->val == control->getMinNode()->val) {
            id_node = id_node->left;
            return *this;
        }
        if (!lef_t) {
            id_node = id_node->left;
            paren_t = false;
            righ_t = true;
            while (id_node->right) { id_node = id_node->right; }
            id_node->left ? lef_t = false : lef_t = true;
            return *this;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->left == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->left ? lef_t = false : lef_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            } else {
                tmp->left ? lef_t = false : lef_t = true;
                righ_t = true;
                id_node = tmp;
            }
            return *this;
        }
    }

    Iterator<T> operator--(int) { // NOLINT
        Iterator helper = *this;
        if (!id_node) {
            id_node = control->getMaxNode();
            return helper;
        }

        init_position_mm();

        if (id_node->val == control->getMinNode()->val) {
            id_node = id_node->left;
            return helper;
        }
        if (!lef_t) {
            id_node = id_node->left;
            paren_t = false;
            righ_t = true;
            while (id_node->right) { id_node = id_node->right; }
            id_node->left ? lef_t = false : lef_t = true;
            return helper;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->left == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->left ? lef_t = false : lef_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            } else {
                tmp->left ? lef_t = false : lef_t = true;
                righ_t = true;
                id_node = tmp;
            }
            return helper;
        }
    }

    const T& operator*() {
        return id_node->val;
    }

    const T *operator->() {
        return &id_node->val;
    }

    friend bool operator!=(const Iterator<T> &b, const Iterator<T> &a) {
        return b.id_node != a.id_node;
    }

    friend bool operator==(const Iterator<T> &b, const Iterator<T> &a) {
        return b.id_node == a.id_node;
    }

    Node<T> *get_node() {
        return id_node;
    }

};

template<typename T>
class rIterator {
private:
    Node<T> *id_node{};
    const Control<T>* control{};
    bool lef_t = false;
    bool righ_t = false;
    bool paren_t = false;

    void init_position_pp() {
        righ_t = true;
        id_node->left ? lef_t = false : lef_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
    }

    void init_position_mm() {
        lef_t = true;
        id_node->right ? righ_t = false : righ_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;

    ~rIterator() = default;

    rIterator() = default;

    explicit rIterator(Node<T> *curr) {
        this->id_node = curr;
        lef_t = false;
        righ_t = false;
        paren_t = false;
    }

    rIterator(const rIterator &it) {
        this->id_node = it.id_node;
        this->control = it.control;
        this->lef_t = it.lef_t;
        this->righ_t = it.righ_t;
        this->paren_t = it.paren_t;
    }

    rIterator(Node<T> *curr, bool flag, const Control<T> *info) {
        control = info;
        if (flag) {
            if (curr->Color != 'W') {
                id_node = control->getMaxNode();
            } else id_node = nullptr;
        } else id_node = nullptr;
    }

    rIterator<T> &operator++() {
        init_position_pp();

        if (id_node->val == control->getMinNode()->val) {
            id_node = id_node->left;
            return *this;
        }
        if (!lef_t) {
            id_node = id_node->left;
            paren_t = false;
            righ_t = true;
            while (id_node->right) { id_node = id_node->right; }
            id_node->left ? lef_t = false : lef_t = true;
            return *this;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->left == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->left ? lef_t = false : lef_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            } else {
                tmp->left ? lef_t = false : lef_t = true;
                righ_t = true;
                id_node = tmp;
            }
            return *this;
        }
    }

    rIterator<T> operator++(int) { // NOLINT

        rIterator helper = *this;

        init_position_pp();

        if (id_node->val == control->getMinNode()->val) {
            id_node = id_node->left;
            return helper;
        }
        if (!lef_t) {
            id_node = id_node->left;
            paren_t = false;
            righ_t = true;
            while (id_node->right) { id_node = id_node->right; }
            id_node->left ? lef_t = false : lef_t = true;
            return helper;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->left == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->left ? lef_t = false : lef_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            } else {
                tmp->left ? lef_t = false : lef_t = true;
                righ_t = true;
                id_node = tmp;
            }
            return helper;
        }
    }

    rIterator<T> &operator--() {
        if (!id_node) {
            id_node = control->getMinNode();
            return *this;
        }
        init_position_mm();

        if (id_node->val == control->getMaxNode()->val) {
            id_node = id_node->right;
            return *this;
        }
        if (!righ_t) {
            id_node = id_node->right;
            paren_t = false;
            lef_t = true;
            while (id_node->left) { id_node = id_node->left; }
            id_node->right ? righ_t = false : righ_t = true;
            return *this;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                tmp->right ? righ_t = false : righ_t = true;
                lef_t = true;
                id_node = tmp;
            } else {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->right == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->right ? righ_t = false : righ_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            }
            return *this;
        }
    }

    rIterator<T> operator--(int) { // NOLINT

        rIterator helper = *this;

        if (!id_node) {
            id_node = control->getMinNode();
            return helper;
        }
        init_position_mm();

        if (id_node->val == control->getMaxNode()->val) {
            id_node = id_node->right;
            return helper;
        }
        if (!righ_t) {
            id_node = id_node->right;
            paren_t = false;
            lef_t = true;
            while (id_node->left) { id_node = id_node->left; }
            id_node->right ? righ_t = false : righ_t = true;
            return helper;
        } else {
            Node<T> *tmp = id_node->parent;
            tmp->parent ? paren_t = false : paren_t = true;
            if (id_node == tmp->left) {
                tmp->right ? righ_t = false : righ_t = true;
                lef_t = true;
                id_node = tmp;
            } else {
                lef_t = true;
                righ_t = true;
                while (tmp) {
                    if (tmp->right == id_node) {
                        id_node = tmp;
                        tmp = tmp->parent;
                        continue;
                    }
                    tmp->right ? righ_t = false : righ_t = true;
                    tmp->parent ? paren_t = false : paren_t = true;
                    break;
                }
                id_node = tmp;
            }
            return helper;
        }
    }

    T *operator->() {
        return &id_node->val;
    }

    const T& operator*() {
        return id_node->val;
    }

    friend bool operator!=(const rIterator<T> &b, const rIterator<T> &a) {
        return b.id_node != a.id_node;
    }

    friend bool operator==(const rIterator<T> &b, const rIterator<T> &a) {
        return b.id_node == a.id_node;
    }

    Node<T> *get_node() {
        return id_node;
    }
};

template<typename T>
class const_Iterator {
private:

    Iterator<T> it;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;

    explicit const_Iterator(Node<T> *node, const Control<T> *info){
        Iterator<T> tmp(node, info);
        it = tmp;
    }

    const_Iterator(Node<T> *root, bool flag, const Control<T>* info) {
        Iterator<T> tmp(root, flag, info);
        it = tmp;
    }
    const_Iterator(const const_Iterator& other) {
        it = other.it;
    }

    const_Iterator& operator=(const const_Iterator& other) {
        if (this != &other) {
            it = other.it;
        }
        return *this;
    }

    const_Iterator &operator++() {
        ++it;
        return *this;
    }

    const_Iterator operator++(int) {//NOLINT
        it++;
        return *this;
    }

    const_Iterator &operator--() {
        --it;
        return *this;
    }

    const_Iterator operator--(int) {//NOLINT
        it--;
        return *this;
    }

    const T& operator*() {
        return *it;
    }

    const T *operator->() {
        return &(it.id_node->val);
    }

    friend bool operator!=(const const_Iterator<T> &a, const const_Iterator<T> &b) {
        return a.it != b.it;
    }

    friend bool operator==(const const_Iterator<T> &a, const const_Iterator<T> &b) {
        return a.it == b.it;
    }

    Node<T>* get_node(){
        return it.get_node();
    }

};

template<typename T>
class const_rIterator {
private:
    rIterator<T> it;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;

    const_rIterator(Node<T> *root, bool flag, const Control<T> *info) {
        rIterator<T> tmp(root, flag, info);
        it = tmp;
    }

    explicit const_rIterator(Node<T>* curr){
        rIterator<T> tmp(curr);
        it = tmp;
    }

    const_rIterator &operator++() {
        ++it;
        return *this;
    }

    const_rIterator operator++(int) {//NOLINT
        it++;
        return *this;
    }

    const_rIterator &operator--() {
        --it;
        return *this;
    }

    const_rIterator operator--(int) {//NOLINT
        it--;
        return *this;
    }

    const T& operator*() {
        return *it;
    }

    const T *operator->() {
        return &(it.id_node->val);
    }

    friend bool operator!=(const const_rIterator<T> &a, const const_rIterator<T> &b) {
        return a.it != b.it;
    }

    friend bool operator==(const const_rIterator<T> &a, const const_rIterator<T> &b) {
        return a.it == b.it;
    }

    Node<T>* get_node(){
        return it.get_node();
    }
};


/*  Реализация к-ч дерева.
 * Тут представлена реализация большинства методов которые есть в классе set. Другие методы реализованы уже в самом классе set.
 * Внутри класса я напишу какие методы за что отвечаю, где используются и распишу алгоритм некоторых методов. */
template<typename T>
class RBTree {
private:
    Node<T> *root;

    size_t size{0};

    Control<T> control;

    // используется для метода print
    void print_node(Node<T> *curr) const { // NOLINT(misc-no-recursion)
        if (!curr) return;
        print_node(curr->left);
        cout << curr->val << " ";
        print_node(curr->right);
    }

    // используется для метода clear и деструктора.
    void delete_node(Node<T> *curr) { // NOLINT(misc-no-recursion)
        if (!curr) return;
        delete_node(curr->left);
        delete_node(curr->right);
        delete curr;
        root = nullptr;
    }

    // используется в балансировках дерева.
    static char getColor(Node<T> *curr) {
        if (curr == nullptr) return 'B';
        return curr->Color;
    }

    // используется в балансировках дерева.
    static void setColor(Node<T> *curr, char s) {
        if (curr == nullptr) return;
        curr->Color = s;
    }

    // используется в балансировках дерева.
    void rotateLeft(Node<T> *ptr) {
        Node<T> *right_child = ptr->right;
        ptr->right = right_child->left;

        if (ptr->right != nullptr)
            ptr->right->parent = ptr;

        right_child->parent = ptr->parent;

        if (ptr->parent == nullptr)
            root = right_child;
        else if (ptr == ptr->parent->left)
            ptr->parent->left = right_child;
        else
            ptr->parent->right = right_child;

        right_child->left = ptr;
        ptr->parent = right_child;
    }

    // используется в балансировках дерева.
    void rotateRight(Node<T> *ptr) {
        Node<T> *left_child = ptr->left;
        ptr->left = left_child->right;

        if (ptr->left != nullptr)
            ptr->left->parent = ptr;

        left_child->parent = ptr->parent;

        if (ptr->parent == nullptr)
            root = left_child;
        else if (ptr == ptr->parent->left)
            ptr->parent->left = left_child;
        else
            ptr->parent->right = left_child;

        left_child->right = ptr;
        ptr->parent = left_child;
    }

    // балансировка при вставке
    void fixInsertRBTree(Node<T> *ptr) {
        Node<T> *parent;
        Node<T> *grandparent;
        while (ptr != root && getColor(ptr) == 'R' && getColor(ptr->parent) == 'R') {
            parent = ptr->parent;
            grandparent = parent->parent;
            if (parent == grandparent->left) {
                Node<T> *uncle = grandparent->right;
                if (getColor(uncle) == 'R') {
                    setColor(uncle, 'B');
                    setColor(parent, 'B');
                    setColor(grandparent, 'R');
                    ptr = grandparent;
                } else {
                    if (ptr == parent->right) {
                        rotateLeft(parent);
                        ptr = parent;
                        parent = ptr->parent;
                    }
                    rotateRight(grandparent);
                    swap(parent->Color, grandparent->Color);
                    ptr = parent;
                }
            } else {
                Node<T> *uncle = grandparent->left;
                if (getColor(uncle) == 'R') {
                    setColor(uncle, 'B');
                    setColor(parent, 'B');
                    setColor(grandparent, 'R');
                    ptr = grandparent;
                } else {
                    if (ptr == parent->left) {
                        rotateRight(parent);
                        ptr = parent;
                        parent = ptr->parent;
                    }
                    rotateLeft(grandparent);
                    swap(parent->Color, grandparent->Color);
                    ptr = parent;
                }
            }
        }
        setColor(root, 'B');
    }

    // балансировка при удалении (так-то и есть само удаление)
    void fixDeleteRBTree(Node<T> *node) {

        if (node == nullptr || node->Color == 'W')
            return;

        if (node == root) {
            if(node->left == nullptr && node->right != nullptr){
                Node<T>* tmp = node->right;
                delete(root);
                root = tmp;
                root->parent = nullptr;
                setColor(root, 'B');
                size--;
                return;
            }
            else if(node->left != nullptr && node->right == nullptr){
                Node<T>* tmp = node->left;
                delete(root);
                root = tmp;
                root->parent = nullptr;
                setColor(root, 'B');
                size--;
                return;
            }
            delete(root);
            root = new Node<T>{'W'};
            size--;
            return;
        }

        if (getColor(node) == 'R' || getColor(node->left) == 'R' || getColor(node->right) == 'R') {
            Node<T> *child = node->left != nullptr ? node->left : node->right;

            if (node == node->parent->left) {
                node->parent->left = child;
                if (child != nullptr)
                    child->parent = node->parent;
                setColor(child, 'B');
                delete (node);
                size--;
            } else {
                node->parent->right = child;
                if (child != nullptr)
                    child->parent = node->parent;
                setColor(child, 'B');
                delete (node);
                size--;
            }
        } else {
            Node<T> *sibling = nullptr;
            Node<T> *parent = nullptr;
            Node<T> *ptr = node;
            setColor(ptr, 'b');
            while (ptr != root && getColor(ptr) == 'b') {
                parent = ptr->parent;
                if (ptr == parent->left) {
                    sibling = parent->right;
                    if (getColor(sibling) == 'R') {
                        setColor(sibling, 'B');
                        setColor(parent, 'R');//
                        rotateLeft(parent);
                    } else {
                        if (getColor(sibling->left) == 'B' && getColor(sibling->right) == 'B') {
                            setColor(sibling, 'R');//
                            if (getColor(parent) == 'R')
                                setColor(parent, 'B');
                            else
                                setColor(parent, 'b');
                            ptr = parent;
                        } else {
                            if (getColor(sibling->right) == 'B') {
                                setColor(sibling->left, 'B');
                                setColor(sibling, 'R');
                                rotateRight(sibling);
                                sibling = parent->right;
                            }
                            setColor(sibling, parent->Color);
                            setColor(parent, 'B');
                            setColor(sibling->right, 'B');
                            rotateLeft(parent);
                            break;
                        }
                    }
                } else {
                    sibling = parent->left;//
                    if (getColor(sibling) == 'R') {
                        setColor(sibling, 'B');
                        setColor(parent, 'R');
                        rotateRight(parent);
                    } else {
                        if (getColor(sibling->left) == 'B' && getColor(sibling->right) == 'B') {
                            setColor(sibling, 'R');
                            if (getColor(parent) == 'R')
                                setColor(parent, 'B');
                            else
                                setColor(parent, 'b');
                            ptr = parent;
                        } else {
                            if (getColor(sibling->left) == 'B') {
                                setColor(sibling->right, 'B');
                                setColor(sibling, 'R');
                                rotateLeft(sibling);
                                sibling = parent->left;
                            }
                            setColor(sibling, parent->Color);
                            setColor(parent, 'B');
                            setColor(sibling->left, 'B');
                            rotateRight(parent);
                            break;
                        }
                    }
                }
            }
            if (node == node->parent->left)
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
            delete (node);
            setColor(root, 'B');
            size--;
        }
    }

    // балансировка и отвязывание узла от дерева.
    Node<T>* fixExtractRBTree(Node<T> *node) {

        if (node == nullptr || node->Color == 'W')
            return node;

        if (node == root) {
            if(node->left == nullptr && node->right != nullptr){
                root = root->right;
                root->parent = nullptr;
                setColor(root, 'B');
                size--;
                node->right = nullptr;
                return node;
            }
            else if(node->left != nullptr && node->right == nullptr){
                root = root->left;
                root->parent = nullptr;
                setColor(root, 'B');
                size--;
                node->left = nullptr;
                return node;
            }
            root = new Node<T>{'W'};
            size--;
            return node;
        }

        if (getColor(node) == 'R' || getColor(node->left) == 'R' || getColor(node->right) == 'R') {
            Node<T> *child = node->left != nullptr ? node->left : node->right;

            if (node == node->parent->left) {
                node->parent->left = child;
                if (child != nullptr)
                    child->parent = node->parent;
                setColor(child, 'B');
                size--;
                return node;
            } else {
                node->parent->right = child;
                if (child != nullptr)
                    child->parent = node->parent;
                setColor(child, 'B');
                size--;
                return node;
            }
        } else {
            Node<T> *sibling = nullptr;
            Node<T> *parent = nullptr;
            Node<T> *ptr = node;
            setColor(ptr, 'b');
            while (ptr != root && getColor(ptr) == 'b') {
                parent = ptr->parent;
                if (ptr == parent->left) {
                    sibling = parent->right;
                    if (getColor(sibling) == 'R') {
                        setColor(sibling, 'B');
                        setColor(parent, 'R');//
                        rotateLeft(parent);
                    } else {
                        if (getColor(sibling->left) == 'B' && getColor(sibling->right) == 'B') {
                            setColor(sibling, 'R');//
                            if (getColor(parent) == 'R')
                                setColor(parent, 'B');
                            else
                                setColor(parent, 'b');
                            ptr = parent;
                        } else {
                            if (getColor(sibling->right) == 'B') {
                                setColor(sibling->left, 'B');
                                setColor(sibling, 'R');
                                rotateRight(sibling);
                                sibling = parent->right;
                            }
                            setColor(sibling, parent->Color);
                            setColor(parent, 'B');
                            setColor(sibling->right, 'B');
                            rotateLeft(parent);
                            break;
                        }
                    }
                } else {
                    sibling = parent->left;//
                    if (getColor(sibling) == 'R') {
                        setColor(sibling, 'B');
                        setColor(parent, 'R');
                        rotateRight(parent);
                    } else {
                        if (getColor(sibling->left) == 'B' && getColor(sibling->right) == 'B') {
                            setColor(sibling, 'R');
                            if (getColor(parent) == 'R')
                                setColor(parent, 'B');
                            else
                                setColor(parent, 'b');
                            ptr = parent;
                        } else {
                            if (getColor(sibling->left) == 'B') {
                                setColor(sibling->right, 'B');
                                setColor(sibling, 'R');
                                rotateLeft(sibling);
                                sibling = parent->left;
                            }
                            setColor(sibling, parent->Color);
                            setColor(parent, 'B');
                            setColor(sibling->left, 'B');
                            rotateRight(parent);
                            break;
                        }
                    }
                }
            }
            if (node == node->parent->left)
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
            setColor(root, 'B');
            size--;
            return node;
        }
    }

    // необходимый метод для удаления узла (находит узел, который будем удалять по определенным условиям)
    Node<T> *deleteBST(Node<T> *curr, T data) { // NOLINT(misc-no-recursion)
        if (curr == nullptr || curr->Color == 'W')
            return curr;
        if (data < curr->val)
            return deleteBST(curr->left, data);
        if (data > curr->val)
            return deleteBST(curr->right, data);
        if (curr->left == nullptr || curr->right == nullptr)
            return curr;
        Node<T> *temp = minValueNode(curr->right);
        curr->val = temp->val;
        swapNode(curr, temp);
        return deleteBST(temp->right, temp->val);
    }

    // метод, который меняет два узла в дереве местами (используется в deleteBST)
    void swapNode(Node<T> *node1, Node<T> *node2){
        if(node1 == root) root = node2;

        auto color = node1->Color;
        node1->Color = node2->Color;
        node2->Color = color;

        auto *tmp = new Node<T> {'T'};
        tmp->left = node1->left;
        tmp->right = node1->right;

        node1->left = node2->left;
        node1->right = node2->right;
        if(node1->left) node1->left->parent = node1;
        if(node1->right) node1->right->parent = node1;

        node2->left = tmp->left;
        node2->right = tmp->right;
        if(node2->left) node2->left->parent = node2;
        if(node2->right) node2->right->parent = node2;

        if(node1->parent) {
            if (node1->parent->left == node1) {
                node1->parent->left = node2;
            } else {
                node1->parent->right = node2;
            }
        }
        if(node2->parent) {
            if (node2->parent->left == node2) {
                node2->parent->left = node1;
            } else {
                node2->parent->right = node1;
            }
        }
        tmp->parent = node1->parent;
        node1->parent = node2->parent;
        node2->parent = tmp->parent;
        delete tmp;
    }

    // минимальный элемент поддерева (используется в методе deleteBST)
    static Node<T> *minValueNode(Node<T> *node) {
        Node<T> *ptr = node;
        while (ptr->left != nullptr)
            ptr = ptr->left;
        return ptr;
    }

    // максимальный элемент поддерева (не используется, но нужен если реализация удаления будет иной)
    static Node<T> *maxValueNode(Node<T> *node) {
        Node<T> *ptr = node;
        while (ptr->right != nullptr)
            ptr = ptr->right;
        return ptr;
    }

    // используется для оператора присваивания и конструктора копирования
    void make_copy(Node<T> *curr) { // NOLINT(misc-no-recursion)
        if (!curr) return;
        make_copy(curr->left);
        insert(curr->val);
        make_copy(curr->right);
    }

    // Используется для удаления за константное время (без поиска). Используется в erase
    void constDeleteValue(Node<T> *curr, T data){
        Node<T> *node = deleteBST(curr, data);
        if(node) node->val = data;
        control.deletion(node);
        fixDeleteRBTree(node);
    }

    // Используется для отвязывания узла за константное время (без поиска). Используется в extract
    Node<T>* constExtractValue(Node<T> * curr,T data) {
        Node<T> *node = deleteBST(curr, data);
        if(node) node->val = data;
        control.deletion(node);
        fixExtractRBTree(node);

        node->Color = 'R';
        node->parent = nullptr;
        node->right = nullptr;
        node->left = nullptr;
        return node;
    }

    // используется для метода merge (возвращает узел относительно которого будет производиться вставка другого узла)
    Node<T>* nodeForInsert(const T& num){
        Node<T> *curr = root;
        while (curr && curr->val != num) {
            if (curr->val > num && curr->left == nullptr) {
                return curr;
            }
            if (curr->val < num && curr->right == nullptr) {
                return curr;
            }
            if (curr->val > num) {
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return curr;
    }

public:


    /* Реализация итераторов на данный момент требует доработки. Они работают, но тк я смог улучшить их и исправить некоторые трудности
     * теперь нет необходимости в некоторых методах, операциях и прочих штуках. Переделывать сейчас это не вижу первостепенной задачей
     * так как работает без багов. Может быть потом уберу из них все лишнее. */

    Iterator<T> begin()  { return {root, true, &control}; }

    [[nodiscard]] const_Iterator<T> cbegin() const { return {root, true, &control};}

    Iterator<T> end()  { return {root, false, &control}; }

    [[nodiscard]] const_Iterator<T> cend() const {return {root, false, &control}; }

    rIterator<T> rbegin()  { return {root, true, &control}; }

    [[nodiscard]] const_rIterator<T> crbegin() const { return {root, true, &control};}

    rIterator<T> rend()  { return {root, false, &control}; }

    [[nodiscard]] const_rIterator<T> crend() const {return {root, false, &control}; }


    /* Данные методы, представляют собой обычный поиск в бинарном дереве и некоторые условия. */

    Iterator<T> find(const T &num)  {
        Node<T> *curr = root;
        while(curr){
            if(curr->val == num){
                Iterator<T> it(curr, &control);
                return it;
            }
            if(curr->val > num){
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return end();
    }

    [[nodiscard]] const_Iterator<T> cfind(const T &num) const {
        Node<T> *curr = root;
        while(curr){
            if(curr->val == num){
                const_Iterator<T> it(curr, &control);
                return it;
            }
            if(curr->val > num){
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return cend();
    }

    Iterator<T> lower_bound(const T& val){
        Iterator<T> it(root, &control);
        while(it != end()){
            if(*it == val) break;
            if(*it < val) {
                ++it;
                continue;
            }
            if(*it > val){
                --it;
                if(*it < val) {
                    ++it;
                    break;
                }
                continue;
            }
        }
        return it;
    }

    [[nodiscard]] const_Iterator<T> lower_bound(const T& val) const {
        const_Iterator<T> it(root, &control);
        while(it != end()){
            if(*it == val) break;
            if(*it < val) {
                ++it;
                continue;
            }
            if(*it > val){
                --it;
                if(*it < val) {
                    ++it;
                    break;
                }
                continue;
            }
        }
        return it;
    }

    Iterator<T> upper_bound(const T& val){
        Iterator<T> it(root, &control);
        while(it != end()){
            if(*it == val) {
                ++it;
                break;
            }
            if(*it < val) {
                ++it;
                continue;
            }
            if(*it > val){
                --it;
                if(*it < val) {
                    ++it;
                    break;
                }
                continue;
            }
        }
        return it;
    }

    [[nodiscard]] const_Iterator<T> upper_bound(const T& val) const{
        const_Iterator<T> it(root, &control);
        while(it != end()){
            if(*it == val) {
                ++it;
                break;
            }
            if(*it < val) {
                ++it;
                continue;
            }
            if(*it > val){
                --it;
                if(*it < val) {
                    ++it;
                    break;
                }
                continue;
            }
        }
        return it;
    }


    // Коротко о методе erase
    // нашли что удалять, обновили класс Control и балансируем с удалением элемента. Возвращаем 0 либо 1.
    size_t erase(const T& data)  {
        size_t tmp = size;
        Node<T> *node = deleteBST(root, data);
        if(node) node->val = data;
        control.deletion(node);
        fixDeleteRBTree(node);
        if(tmp - size) return 1;
        else return 0;
    }

    // Тут прибегаем к фактически обычному erase только без поиска.
    Iterator<T> erase(Iterator<T>& it){
        if(it != end()) {
            Iterator<T> tmp (it);
            ++tmp;
            constDeleteValue(it.get_node(), it.get_node()->val);
            return tmp;
        }
        return it;
    }
    // тут такая же история.
    const_Iterator<T> erase(const_Iterator<T>& it){
        if(it != cend()) {
            const_Iterator<T> tmp (it);
            ++tmp;
            constDeleteValue(it.get_node(), it.get_node()->val);
            return tmp;
        }
        return it;
    }

    // В данном варианте метода используется уже готовый метод erase по итератору используя его в цикле.
    Iterator<T> erase(Iterator<T>& first, Iterator<T>& last){
        while(first != last)
            first = erase(first);
        return last;
    }
    // аналогично.
    const_Iterator<T> erase(const_Iterator<T>& first, const_Iterator<T>& last){
        while(first != last)
            first = erase(first);
        return last;
    }

    // Аналогично erase только узел не удаляется, а отвязывается.
    Node<T> *extract(const T& data) {
        Node<T> *node = deleteBST(root, data);
        if(node) node->val = data;
        control.deletion(node);
        fixExtractRBTree(node);

        node->Color = 'R';
        node->parent = nullptr;
        node->right = nullptr;
        node->left = nullptr;
        return node;
    }
    // То же самое, только отсутствует поиска узла.
    Node<T> * extract(const_Iterator<T>& it){
        return constExtractValue(it.get_node(), it.get_node()->val);
    }

    // Алгоритм данного метода выглядит следующим образом:
    //  1. Создаем итератор begin для источника и в цикле проходимся по дереву.
    //  2. Если значение больше либо меньше минимального или максимально значения в дереве, то из источника берется узел и вставляется в дерево.
    //     Данное условие можно ускорить с log n до O(1) если использовать инсерт по итератору.
    //  3. Используем специальный метод, получаем узел относительно которого можно вставить узел по итератору.
    //  4. Если значение узла != значению узла по итератору и узел действительный, тогда берем узел из источника и связываем с деревом.
    void merge(RBTree& other){
        const_Iterator<T> it(other.control.getMinNode(), &other.control);
        while(it != other.cend()){
            const_Iterator<T> curr(it);
            it++;
            if(curr.get_node()->val < control.getMinNode()->val || curr.get_node()->val > control.getMaxNode()->val){
                insert(other.extract(curr));
            }
            else{
                Node<T> *position = nodeForInsert(curr.get_node()->val);
                if(position != nullptr){
                    if(curr.get_node()->val == position->val) continue;
                    if(curr.get_node()->val < position->val) {
                        Node<T> *tmp = other.extract(curr);
                        position->left = tmp;
                        tmp->parent = position;
                        size++;
                        control.addition(tmp);
                        fixInsertRBTree(tmp);
                        continue;
                    }
                    if(curr.get_node()->val > position->val) {
                        Node<T> *tmp = other.extract(curr);
                        position->right = tmp;
                        tmp->parent = position;
                        size++;
                        control.addition(tmp);
                        fixInsertRBTree(tmp);
                        continue;
                    }
                }
                continue;
            }
        }
        if(other.empty()) other.control.clear();
    }


    ~RBTree()  { delete_node(root); }

    RBTree() : root(new Node<T>{'W'}) {}

    RBTree(const T *arr, T n) {
        root = new Node<T>{'W'};
        make_from_array(arr, n);
    }

    RBTree(const std::initializer_list<T>& list) {
        root = new Node<T>{'W'};
        for (const auto &it: list) insert(it);
    }

    RBTree(const RBTree &copy) {
        root = new Node{0, 'W'};
        make_copy(copy.root);
    }

    RBTree(RBTree &other)  {
        root = other.root;
        size = other.size;
        control = other.control;
        other.root = new Node<T>{'W'};
        other.size = 0;
        other.control.clear();
    }


    // Данные методы преимущественно используют одну и туже логику поиска места для вставки в цикле за log(size())
    // Стоит упомянуть только методы insert по итератору (позиции), который в лучшем случае вставляют за константное время.
    // Логика заключается в том, что вставить в дерево не проходясь по нему можно, только если вставляемый элемент
    // больше либо меньше максимального, либо минимального значения в дереве. В иных случаях ищем место для вставки.
    Iterator<T> insert(Iterator<T>& it, const T &num) {
        if(it.get_node() == control.getMinNode() && num < *it){
            it.get_node()->left = new Node<T>{num, 'R'};
            it.get_node()->left->parent = it.get_node();
            control.addition(it.get_node()->left);
            fixInsertRBTree(it.get_node()->left);
            size++;
            return --it;
        }
        else if(it.get_node() == control.getMaxNode() && num > *it){
            it.get_node()->right = new Node<T>{num, 'R'};
            it.get_node()->right->parent = it.get_node();
            control.addition(it.get_node()->right);
            fixInsertRBTree(it.get_node()->right);
            size++;
            return ++it;
        }

        return insert(num).first;
    }

    Iterator<T> insert(const_Iterator<T>& it, const T &num)  {
        if(it.get_node() == control.getMinNode() && num < *it){
            it.get_node()->left = new Node<T>{num, 'R'};
            it.get_node()->left->parent = it.get_node();
            control.addition(it.get_node()->left);
            fixInsertRBTree(it.get_node()->left);
            size++;
            --it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;


        }
        else if(it.get_node() == control.getMaxNode() && num > *it){
            it.get_node()->right = new Node<T>{num, 'R'};
            it.get_node()->right->parent = it.get_node();
            control.addition(it.get_node()->right);
            fixInsertRBTree(it.get_node()->right);
            size++;
            ++it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;
        }
        return insert(num).first;
    }

    Iterator<T> insert(const_Iterator<T>& it, T &&num)  {
        if(it.get_node() == control.getMinNode() && num < *it){
            it.get_node()->left = new Node<T>{num, 'R'};
            it.get_node()->left->parent = it.get_node();
            control.addition(it.get_node()->left);
            fixInsertRBTree(it.get_node()->left);
            size++;
            --it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;
        }
        else if(it.get_node() == control.getMaxNode() && num > *it){
            it.get_node()->right = new Node<T>{num, 'R'};
            it.get_node()->right->parent = it.get_node();
            control.addition(it.get_node()->right);
            fixInsertRBTree(it.get_node()->right);
            size++;
            ++it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;
        }
        return insert(num).first;
    }

    std::pair<Iterator<T>, bool> insert(T &&num){
        Node<T> *curr = root;
        while (curr && curr->val != num) {
            if (curr->Color == 'W') {
                curr->val = num;
                curr->Color = 'B';
                size++;
                control.addition(curr);
                fixInsertRBTree(curr);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val > num && curr->left == nullptr) {
                curr->left = new Node{num, 'R'};
                curr->left->parent = curr;
                size++;
                control.addition(curr->left);
                fixInsertRBTree(curr->left);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val < num && curr->right == nullptr) {
                curr->right = new Node{num, 'R'};
                curr->right->parent = curr;
                size++;
                control.addition(curr->right);
                fixInsertRBTree(curr->right);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val > num) {
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return std::make_pair(this->end(), false);
    }

    std::pair<Iterator<T>, bool> insert(const T &num){
        Node<T> *curr = root;
        while (curr && curr->val != num) {
            if (curr->Color == 'W') {
                curr->val = num;
                curr->Color = 'B';
                size++;
                control.addition(curr);
                fixInsertRBTree(curr);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val > num && curr->left == nullptr) {
                curr->left = new Node{num, 'R'};
                curr->left->parent = curr;
                size++;
                control.addition(curr->left);
                fixInsertRBTree(curr->left);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val < num && curr->right == nullptr) {
                curr->right = new Node{num, 'R'};
                curr->right->parent = curr;
                size++;
                control.addition(curr->right);
                fixInsertRBTree(curr->right);
                return std::make_pair(this->find(num), true);
            }
            if (curr->val > num) {
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return std::make_pair(this->end(), false);
    }

    void insert(std::initializer_list<T>& list) {
        for (auto it: list) {
            insert(it);
        }
    }

    std::pair<Iterator<T>, bool> insert(Node<T>* node){
        Node<T> *curr = root;
        while (curr && curr->val != node->val) {
            if (curr->Color == 'W') {
                curr = root = node;
                curr->Color = 'B';
                size++;
                control.addition(curr);
                fixInsertRBTree(curr);
                return std::make_pair(this->find(node->val), true);
            }
            if (curr->val > node->val && curr->left == nullptr) {
                curr->left = node;
                node->parent = curr;
                size++;
                control.addition(curr->left);
                fixInsertRBTree(curr->left);
                return std::make_pair(this->find(node->val), true);
            }
            if (curr->val < node->val && curr->right == nullptr) {
                curr->right = node;
                node->parent = curr;
                size++;
                control.addition(curr->right);
                fixInsertRBTree(curr->right);
                return std::make_pair(this->find(node->val), true);
            }
            if (curr->val > node->val) {
                curr = curr->left;
            } else
                curr = curr->right;
        }
        return std::make_pair(this->find(node->val), false);
    }

    Iterator<T> insert(const_Iterator<T>& it, Node<T> *node)  {
        if(it.get_node() == control.getMinNode() && node->val < *it){
            it.get_node()->left = node;
            it.get_node()->left->parent = it.get_node();
            control.addition(it.get_node()->left);
            fixInsertRBTree(it.get_node()->left);
            size++;
            --it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;
        }
        else if(it.get_node() == control.getMaxNode() && node->val > *it){
            it.get_node()->right = node;
            it.get_node()->right->parent = it.get_node();
            control.addition(it.get_node()->right);
            fixInsertRBTree(it.get_node()->right);
            size++;
            ++it;
            Iterator<T> tmp(it.get_node(), &control);
            return tmp;
        }
        return insert(node).first;
    }


    // В методах ниже особо нечего обьяснять. Упомяну лишь о том, что состояние пустого дерева, это дерево с 1 узлом цвета 'W'.
    void swap_(RBTree &copy)  {
        Node<T> *tmp1 = copy.root;
        Control<T> tmp2 = copy.control;
        size_t tmp3 = copy.size;

        copy.root = root;
        copy.control = control;
        copy.size = size;

        root = tmp1;
        control = tmp2;
        size = tmp3;
    }

    void clear()  {
        delete_node(root);
        root = new Node<T>('W');
        control.clear();
        size = 0;
    }

    void print() const  {
        Node<T> *curr = root;
        if (curr->Color == 'W') return;
        print_node(curr);
        cout << " ";
    }

    [[nodiscard]] bool contains(const T &num) const {
        Iterator<T> it = find(num);
        if(it == end()) return false;
        else return true;
    }

    [[nodiscard]] size_t count(const T &num) const {
        Iterator<T> it = find(num);
        if(it == end()) return 0;
        else return 1;
    }

    [[nodiscard]] size_t length() const noexcept  {
        return size;
    }

    [[nodiscard]] bool empty() const noexcept  {
        if (root->Color == 'W') return true;
        else return false;
    }

    void make_from_array(const T *array, size_t n)  {
        for (int i = 0; i < n; i++) {
            insert(array[i]);
        }
    }

    static bool is_equal(RBTree<T> &curr, RBTree<T> &ptr) {
        if (curr.length() == ptr.length()) {
            Iterator<T> i(curr.control.getMinNode(), &curr.control);
            Iterator<T> j(ptr.control.getMinNode(), &ptr.control);
            while (i != curr.end() && j != ptr.end()) {
                if (*i != *j) return false;
                else {
                    ++i;
                    ++j;
                }
            }
            return true;
        }
        return false;
    }


    RBTree &operator=(const RBTree<T> &copy) {
        cout<<'Y';
        if (&copy != this) {
            if (!this->empty()) this->clear();
            make_copy(copy.root);
        }
        return *this;
    }

    friend bool operator==(RBTree<T> &left, RBTree<T> &right) {
        return is_equal(left, right);
    }

    friend bool operator!=(RBTree<T> &left, RBTree<T> &right) {
        return !(left == right);
    }

    friend bool operator< (RBTree<T> &left, RBTree<T> &right) {
        Iterator i(left.control.getMinNode(), &left.control);
        Iterator j(right.control.getMinNode(), &right.control);
        while (true) {
            if (i == left.end() && j == right.end()) return true;
            if (i == left.end() && j != right.end()) return true;
            if (i != left.end() && j == right.end()) return false;
            if (*i < *j) return false;
            else {
                ++i;
                ++j;
            }
        }
    }

    friend bool operator> (RBTree<T> &left, RBTree<T> &right) {
        return right < left;
    }

    friend bool operator<=(RBTree<T> &left, RBTree<T> &right) {
        return right >= left;
    }

    friend bool operator>=(RBTree<T> &left, RBTree<T> &right) {
        return left >= right;
    }
};

// объект возвращаемый при вставке узла в контейнер (дескриптор узла)
template<typename T>
struct insert_return_type{
    bool inserted = false;
    Iterator<T> position;
    Node<T>* node{};
};


// Основной класс.
template<typename T>
class set {
private:

    RBTree<T> *rbTree;

public:

/*   Конструкторы и деструктор.
 *   Реализованно шесть конструкторов: дефолтный, копирования, перемещения, иншлайзер лист, с двумя итераторами, диапазон */

    ~set(){
        rbTree->~RBTree();
        delete rbTree;
    }

    set() { rbTree = new RBTree<T> (); }

    set(const set &other) { rbTree = new RBTree<T> (*other.rbTree); }

    set(set &&other) noexcept {
        rbTree = new RBTree<T> (*other.rbTree);
    }

    set(const std::initializer_list<T> list) {
        rbTree = new RBTree<T> (list);
    }

    template<class InputIt>
    set(InputIt first, InputIt last){
        rbTree = new RBTree<T> ();
        insert(first, last);
    }

    template <typename R, typename = container_compatible_range<R>>
    explicit set(R&& rg){
        rbTree = new RBTree<T> ();
        for(auto it = rg.begin(); it != rg.end(); ++it){
            insert(*it);
        }
    }



/*   Методы "Iterators".
 *  Реализованно все 4 вида итераторов. Обычные, реверс и аналогичные константные итераторы.
 *  Скорость методов O(1), что соответствует стандарту. */

    Iterator<T> begin() { return rbTree->begin(); }

    [[nodiscard]] const_Iterator<T> cbegin() const { return rbTree->cbegin(); }

    Iterator<T> end() { return rbTree->end(); }

    [[nodiscard]] const_Iterator<T> cend() const { return rbTree->cend();}

    rIterator<T> rbegin() { return rbTree->rbegin(); }

    [[nodiscard]] const_rIterator<T> crbegin() const { return rbTree->crbegin();}

    rIterator<T> rend() { return rbTree->rend(); }

    [[nodiscard]] const_rIterator<T> crend() const { return rbTree->crend();}




/*   Методы "Capacity"
 *  Скорость у всех методов O(1), что соответствует стандарту. */

    [[nodiscard]] bool empty() const noexcept {return rbTree->empty();}

    [[nodiscard]] size_t size() const noexcept { return rbTree->length();}

    [[nodiscard]] size_t max_size() const noexcept {return std::numeric_limits<T>::max();}




/*   Методы "Lookup"
 *  Реализованны все методы данного типа. Скорость как в стандарте у всех методов O(log(N)) , где N = size() */

    Iterator<T> find(const T &val) { return rbTree->find(val); }

    [[nodiscard]] const_Iterator<T> cfind(const T &val) const{ return rbTree->cfind(val); }

    Iterator<T> lower_bound(const T& val) { return rbTree->lower_bound(val); }

    [[nodiscard]] const_Iterator<T> lower_bound(const T& val) const { return rbTree->lower_bound(val); }

    Iterator<T> upper_bound(const T& val) { return rbTree->upper_bound(val); }

    [[nodiscard]] const_Iterator<T> upper_bound(const T& val) const{ return rbTree->upper_bound(val); }

    std::pair<Iterator<T>,Iterator<T>> equal_range( const T& val ){return std::make_pair(lower_bound(val), upper_bound(val));}

    [[nodiscard]] std::pair<const_Iterator<T>,const_Iterator<T>> equal_range( const T& val ) const { return std::make_pair(lower_bound(val), upper_bound(val));}

    [[nodiscard]] size_t count(const T &val) const { return rbTree->count(val); }

    [[nodiscard]] bool contains(const T &val) const { return rbTree->contains(val); }




/*   Методы "Modifiers"
 *  Реализованны все методы данного типа. Скорость каждого метода и его различных вариантов соответствует стандарту. */

    // O(N*log(a.size()+N)), где N это размер диапазона.
    template <typename R, typename = container_compatible_range<R>>
    void insert_range(R&& rg){
        for(auto it = rg.begin(); it != rg.end(); ++it){
                insert(*it);
        }
    }

    // O(N*log(a.size()+N)), где N равен количеству элементов которые необходимо вставить в контейнер.
    template<class InputIt>
    void insert(InputIt first, InputIt last) {
            while (first != last) {
                insert(*first);
                ++first;
            }
    }

    // O(N*log(a.size()+N)), где N равен количеству элементов которые необходимо вставить в контейнер.
    void insert(std::initializer_list<T> list) { rbTree->insert(list); }

    // O(log(size())).
    insert_return_type<T> insert(Node<T> *nh){
        if(nh == nullptr) {
            insert_return_type<T> recovery;
            return recovery;
        }
        std::pair<Iterator<T>, bool> temp = rbTree->insert(nh);
        if(temp.second == true){
            insert_return_type<T> recovery;
            recovery.inserted = true;
            recovery.position = temp.first;
            return recovery;
        }
        insert_return_type<T> recovery;
        recovery.inserted = false;
        recovery.position = temp.first;
        recovery.node = (--temp.first).get_node();
        return recovery;
    }

    // O(log(size())).
    std::pair<Iterator<T>, bool> insert(T &&val){ return rbTree->insert(val); }

    // O(log(size())).
    std::pair<Iterator<T>, bool> insert(const T &val){ return rbTree->insert(val); }

    // В лучшем случае O(1) в остальном же O(log(size())).
    Iterator<T> insert(Iterator<T> position, const T &val){ return rbTree->insert(position, val); }

    // В лучшем случае O(1) в остальном же O(log(size())).
    Iterator<T> insert(const_Iterator<T> position, const T &val) { return rbTree->insert(position, val); }

    // В лучшем случае O(1) в остальном же O(log(size())).
    Iterator<T> insert(const_Iterator<T> position, T &&val) { return rbTree->insert(position, val); }

    // В лучшем случае O(1) в остальном же O(log(size())).
    Iterator<T> insert(const_Iterator<T> position, Node<T> *nh) { return rbTree->insert(position, nh); }


    // O(log(size())).
    size_t erase(const T& val){ return rbTree->erase(val); }

    // O(1)
    Iterator<T> erase(Iterator<T> position) { return rbTree->erase(position); }

    // O(1)
    const_Iterator<T> erase(const_Iterator<T> position) { return rbTree->erase(position); }

    // У меня O(std::distance(first, last)), в стандарте O(log(c.size()) + std::distance(first, last)) (как так вышло я не знаю =))
    Iterator<T> erase(Iterator<T> first, Iterator<T> last) { return rbTree->erase(first, last); }

    // У меня O(std::distance(first, last)), в стандарте O(log(c.size()) + std::distance(first, last)) (как так вышло я не знаю =))
    const_Iterator<T> erase(const_Iterator<T> first, const_Iterator<T> last) { return rbTree->erase(first, last); }


    // O(log(size())).
    Node<T> *extract(const T& key) { return rbTree->extract(key); }

    // O(1)
    Node<T> *extract(const_Iterator<T> position) { return rbTree->extract(position); }

    // O(N*log(a.size()+N)), где N это размер источника.
    void merge(set<T>& otherSet){ rbTree->merge(*otherSet.rbTree); }

    // O(1)
    void swap(set<T> &otherSet) { rbTree->swap_(*otherSet.rbTree); }

    // O(N), где N размер контейнера.
    void clear() noexcept { rbTree->clear(); }




/*   Non-member functions
 *  Реализованно все кроме космолета. Если разберусь, то он тоже будет. Скорость методов совпадает со скоростью в стандарте. */

    // если размеры контейнеров отличаются, то O(1) иначе O(N) - N размер контейнера.
    friend bool operator==(set<T> &left, set<T> &right) { return *left.rbTree == *right.rbTree; }

    // если размеры контейнеров отличаются, то O(1) иначе O(N) - N размер контейнера.
    friend bool operator!=(set<T> &left, set<T> &right) { return *left.rbTree != *right.rbTree; }

    // O(N) - N размер контейнера.
    friend bool operator<(set<T> &left, set<T> &right) { return *left.rbTree < *right.rbTree; }

    // O(N) - N размер контейнера.
    friend bool operator>(set<T> &left, set<T> &right) { return *left.rbTree > *right.rbTree; }

    // O(N) - N размер контейнера.
    friend bool operator<=(set<T> &left, set<T> &right) { return *left.rbTree <= *right.rbTree; }

    // O(N) - N размер контейнера.
    friend bool operator>=(set<T> &left, set<T> &right) { return *left.rbTree >= *right.rbTree; }

};


int main(int argc, char const *argv[]) {

    set<int> mySet1 = {1, 2, 3, 4, 5};
    set<int> mySet2 = {6, 7, 8, 9, 10};
    set<int> mySet;

    mySet.insert(mySet1.extract(mySet1.cbegin()));
    assert(mySet1.find(1) == mySet1.end());

    mySet.merge(mySet2);
    for(auto& it : mySet) cout << it << ' ';
    cout << std::endl;

    mySet.merge(mySet1);
    for(auto& it : mySet) cout << it << ' ';
    cout << std::endl;

    assert(mySet1.empty());
    assert(mySet2.empty());

    for(auto it = mySet.begin(); it != mySet.end(); ){
        if(*it%2) it = mySet.erase(it);
        else ++it;
    }
    for(auto& it : mySet) cout << it << ' ';
    cout << std::endl;

    mySet1.insert_range(mySet);
    for(auto& it : mySet1) cout << it << ' ';
    cout << std::endl;

    assert(mySet == mySet1);
    assert(mySet1 != mySet2);
    assert(mySet1 > mySet2);

    std::vector<int> vec = {5, 12, 0, 9, -8};
    set<int> mySet3(vec);
    for(auto& it : mySet3) cout << it << ' ';
    cout << std::endl;

    mySet2.insert(mySet3.lower_bound(0), mySet3.upper_bound(10));
    for(auto& it : mySet2) cout << it << ' ';
    cout << std::endl;
    return 0;
}