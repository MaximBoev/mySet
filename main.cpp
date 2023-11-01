#include <iostream>
#include <cassert>

using std::cin;
using std::cout;
using std::swap;

template <typename T>
class RBTree;
template <typename T>
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
template <typename T>
class Iterator {
private:
    Node<T> *id_node{};
    Node<T> *max_node{};
    Node<T> *min_node{};
    bool lef_t = false;
    bool righ_t = false;
    bool paren_t = false;
    bool flag_1 = false;
    bool flag_2 = false;

    static Node<T> *max_Node(Node<T> *curr) {
        while (true) {
            if (curr->right == nullptr) return curr;
            curr = curr->right;
        }
    }

    static Node<T> *min_Node(Node<T> *curr) {
        while (true) {
            if (curr->left == nullptr) return curr;
            curr = curr->left;
        }
    }

    void init_position(){
        id_node->left ? lef_t = false : lef_t = true;
        id_node->right ? righ_t = false : righ_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
    }

    void init_position_pp() {
        lef_t = true;
        id_node->right ? righ_t = false : righ_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
        flag_1 = true;
        flag_2 = false;
    }

    void init_position_mm() {
        righ_t = true;
        id_node->left ? lef_t = false : lef_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
        flag_1 = false;
        flag_2 = true;
    }

public:

    ~Iterator() = default;

    Iterator() = default;

    explicit Iterator(Node<T> * curr){
        this->id_node = curr;
    }

    Iterator(const Iterator<T> &it) {
        this->id_node = it.id_node;
        this->lef_t = it.lef_t;
        this->righ_t = it.righ_t;
        this->paren_t = it.paren_t;
        this->flag_1 = it.flag_1;
        this->flag_2 = it.flag_2;
        this->max_node = it.max_node;
        this->min_node = it.min_node;
    }

    Iterator(Node<T> *curr, bool flag) {
        max_node = max_Node(curr);
        min_node = min_Node(curr);
        if (flag) {
            if (curr->Color != 'W') {
                id_node = min_Node(curr);
            } else id_node = nullptr;
        } else id_node = nullptr;
    }

    Iterator<T> &operator++() {
        if (!flag_1) init_position_pp();

        if (id_node->val == max_node->val) {
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
        if (!flag_1) init_position_pp();

        if (id_node->val == max_node->val) {
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
            id_node = max_node;
            return *this;
        }
        if (!flag_2) init_position_mm();

        if (id_node->val == min_node->val) {
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
            id_node = max_node;
            return helper;
        }
        if (!flag_2) init_position_mm();

        if (id_node->val == min_node->val) {
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

    T operator*() {
        return id_node->val;
    }

    T *operator->() {
        return &id_node->val;
    }

    friend bool operator!=(const Iterator<T> &b, const Iterator<T> &a) {
        return b.id_node != a.id_node;
    }

    friend bool operator==(const Iterator<T> &b, const Iterator<T> &a) {
        return b.id_node == a.id_node;
    }

    Node<T> *get_node(){
        return id_node;
    }

};
template <typename T>
class rIterator {
private:
    Node<T> *id_node{};
    Node<T> *max_node{};
    Node<T> *min_node{};
    bool lef_t = false;
    bool righ_t = false;
    bool paren_t = false;
    bool flag_1 = false;
    bool flag_2 = false;

    static Node<T> *min_Node(Node<T> *curr) {
        while (true) {
            if (curr->left == nullptr) return curr;
            curr = curr->left;
        }
    }

    static Node<T> *max_Node(Node<T> *curr) {
        while (true) {
            if (curr->right == nullptr) return curr;
            curr = curr->right;
        }
    }

    void init_position_pp() {
        righ_t = true;
        id_node->left ? lef_t = false : lef_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
        flag_1 = true;
        flag_2 = false;
    }

    void init_position_mm() {
        lef_t = true;
        id_node->right ? righ_t = false : righ_t = true;
        id_node->parent ? paren_t = false : paren_t = true;
        flag_1 = false;
        flag_2 = true;
    }

public:

    ~rIterator() = default;

    rIterator() = default;

    explicit rIterator(Node<T> * curr){
        this->id_node = curr;
    }

    rIterator(const rIterator &it) {
        this->id_node = it.id_node;
        this->lef_t = it.lef_t;
        this->righ_t = it.righ_t;
        this->paren_t = it.paren_t;
        this->flag_1 = it.flag_1;
        this->flag_2 = it.flag_2;
        this->max_node = it.max_node;
        this->min_node = it.min_node;
    }

    rIterator(Node<T> *curr, bool flag) {
        max_node = max_Node(curr);
        min_node = min_Node(curr);
        if (flag) {
            if (curr->Color != 'W') {
                id_node = max_Node(curr);
            } else id_node = nullptr;
        } else id_node = nullptr;
    }

    rIterator<T> &operator++() {
        if (!flag_1) init_position_pp();
        if (id_node->val == min_node->val) {
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

        if (!flag_1) init_position_pp();

        if (id_node->val == min_node->val) {
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
            id_node = min_node;
            return *this;
        }
        if (!flag_2) init_position_mm();
        if (id_node->val == max_node->val) {
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
            id_node = min_node;
            return helper;
        }
        if (!flag_2) init_position_mm();
        if (id_node->val == max_node->val) {
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

    T operator*() {
        return id_node->val;
    }

    friend bool operator!=(const rIterator<T> &b, const rIterator<T> &a) {
        return b.id_node != a.id_node;
    }

    friend bool operator==(const rIterator<T> &b, const rIterator<T> &a) {
        return b.id_node == a.id_node;
    }

    Node<T> *get_node(){
        return id_node;
    }
};
template <typename T>
class abstract_data_t {
public:
    abstract_data_t() = default;

    virtual ~abstract_data_t() = 0;

    virtual bool empty() = 0;

    virtual void clear() = 0;

    virtual void print() const = 0;

    virtual size_t count(T num) = 0;

    virtual bool contains(T num) = 0;

    virtual void insert(T num) = 0;

    virtual void deleteValue(T data) = 0;

    virtual void swap_(RBTree<T> &copy) = 0;

    [[nodiscard]] virtual size_t length() const = 0;

    virtual void make_from_array(const T *array, size_t n) = 0;

    virtual bool is_equal(RBTree<T> &curr, RBTree<T> &ptr) = 0;

    virtual Iterator<T> begin() = 0;

    virtual Iterator<T> end() = 0;

    virtual rIterator<T> rbegin() = 0;

    virtual rIterator<T> rend() = 0;

    virtual Iterator<T> find(const T& num)  = 0;

    virtual rIterator<T> rfind(const T& num) = 0;

    virtual Iterator<T> insert(Iterator<T> it, const T& num) = 0;

    virtual rIterator<T> insert(rIterator<T> it, const T& num) = 0;

    virtual Iterator<T> erase(Iterator<T> it) = 0;

    virtual rIterator<T> erase(rIterator<T> it) = 0;
};
template <typename T>
class RBTree : public abstract_data_t<T> {
private:
    Node<T> *root;
    size_t size{0};

    void print_node(Node<T> *curr) const { // NOLINT(misc-no-recursion)
        if (!curr) return;
        print_node(curr->left);
        cout << curr->val << " ";
        print_node(curr->right);
    }

    void delete_node(Node<T> *curr) { // NOLINT(misc-no-recursion)
        if (!curr) return;
        delete_node(curr->left);
        delete_node(curr->right);
        delete curr;
        root = nullptr;
    }

    static char getColor(Node<T> *curr) {
        if (curr == nullptr) return 'B';
        return curr->Color;
    }

    static void setColor(Node<T> *curr, char s) {
        if (curr == nullptr) return;
        curr->Color = s;
    }

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

    void fixDeleteRBTree(Node<T> *node) {
        if (node == nullptr || node->Color == 'W')
            return;

        if (node == root) {
            root = nullptr;
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
            } else {
                node->parent->right = child;
                if (child != nullptr)
                    child->parent = node->parent;
                setColor(child, 'B');
                delete (node);
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
                        setColor(parent, 'B');
                        rotateLeft(parent);
                    } else {
                        if (getColor(sibling->left) == 'B' && getColor(sibling->right) == 'B') {
                            setColor(sibling, 'B');
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
                    sibling = parent->left;
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
        }
    }

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
        return deleteBST(curr->right, temp->val);
    }

    static Node<T> *minValueNode(Node<T> *node) {
        Node<T> *ptr = node;
        while (ptr->left != nullptr)
            ptr = ptr->left;
        return ptr;
    }

    static Node<T> *maxValueNode(Node<T> *node) {
        Node<T> *ptr = node;
        while (ptr->right != nullptr)
            ptr = ptr->right;
        return ptr;
    }

    void insert_(T num) {
        Node<T> *curr = root;
        while (curr && curr->val != num) {
            if (curr->Color == 'W') {
                curr->val = num;
                curr->Color = 'R';
                size++;
                fixInsertRBTree(curr);
                return;
            }
            if (curr->val > num && curr->left == nullptr) {
                curr->left = new Node{num, 'R'};
                curr->left->parent = curr;
                size++;
                fixInsertRBTree(curr->left);
                return;
            }
            if (curr->val < num && curr->right == nullptr) {
                curr->right = new Node{num, 'R'};
                curr->right->parent = curr;
                size++;
                fixInsertRBTree(curr->right);
                return;
            }
            if (curr->val > num) {
                curr = curr->left;
            } else
                curr = curr->right;
        }

    }

    void make_copy(Node<T> *curr) { // NOLINT(misc-no-recursion)
        if (!curr) return;
        make_copy(curr->left);
        insert(curr->val);
        make_copy(curr->right);
    }

    void make_from_array_(const T *array, size_t n) {
        for (int i = 0; i < n; i++) {
            insert(array[i]);
        }
    }

    void print_ct(std::ostream &l, Node<T> *curr) const {// NOLINT(misc-no-recursion
        if (!curr) return;
        print_ct(l, curr->left);
        if (curr->Color == 'W') return;
        else l << curr->val << ' ';
        print_ct(l, curr->right);
    }

    bool is_equal(RBTree &curr, RBTree &ptr) override {
        if (curr.length() == ptr.length()) {
            Iterator i = curr.begin();
            Iterator j = ptr.begin();
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

public:
    Iterator<T> begin() override { return {root, true}; }

    Iterator<T> end() override { return {root, false}; }

    rIterator<T> rbegin() override { return {root, true}; }

    rIterator<T> rend() override { return {root, false}; }

    Iterator<T> find(const T& num) override {
        for(Iterator<T> it = this->begin(); it != this->end(); ++it){
            if(*it == num) return it;
        }
        return end();
    }

    rIterator<T> rfind(const T& num) override {
        for(rIterator<T> it = this->rbegin(); it != this->rend(); ++it){
            if(*it == num) return it;
        }
        return rend();
    }

    Iterator<T> insert(Iterator<T> it, const T& num) override{
        if(it.get_node() == nullptr || it.get_node()->Color == 'W') {
            insert(num);
            return find(num);
        }
        if(*it > num && *(--it) < num){
            ++it;
            Node<T> *tmp = it.get_node();
            tmp->left = new Node<T>{num, 'R'};
            tmp->left->parent = tmp;
            Iterator<T> curr(tmp->left);
            fixInsertRBTree(tmp->left);
            size++;
            return curr;
        } else{
            insert(num);
            return find(num);
        }
    }

    rIterator<T> insert(rIterator<T> it, const T& num) override{
        if(it.get_node() == nullptr || it.get_node()->Color == 'W') {
            insert(num);
            return rfind(num);
        }
        if(*it > num && *(--it) < num){
            ++it;
            Node<T> *tmp = it.get_node();
            tmp->left = new Node<T>{num, 'R'};
            tmp->left->parent = tmp;
            rIterator<T> curr(tmp->left);
            fixInsertRBTree(tmp->left);
            size++;
            return curr;
        } else{
            insert(num);
            return rfind(num);
        }
    }

    Iterator<T> erase(Iterator<T> it) override {
        Iterator<T> tmp = it;
        ++it;
        deleteValue(*tmp);
        return it;
    }

    rIterator<T> erase(rIterator<T> it) override {
        rIterator<T> tmp = it;
        ++it;
        deleteValue(*tmp);
        return it;
    }

    RBTree() : root(new Node<T>{'W'}) {}

    RBTree(const T *arr, T n) {
        root = new Node<T>{'W'};
        make_from_array_(arr, n);
    }

    RBTree(const std::initializer_list<T> list){
        root = new Node<T>{'W'};
        for(const auto& it : list) insert_(it);
    }

    RBTree(RBTree &copy) {
        root = new Node{0, 'W'};
        make_copy(copy.root);
    }

    RBTree(const Iterator<T> &begin, const Iterator<T> &end) {
        root = new Node(0, 'W');
        for (auto it = begin; it != end; ++it) {
            insert_(*it);
        }
    }

    RBTree(const rIterator<T> &rbegin, const rIterator<T> &rend) {
        root = new Node(0, 'W');
        for (auto it = rbegin; it != rend; ++it) {
            insert_(*it);
        }
    }

    ~RBTree() override { delete_node(root); }

    bool empty() override {
        if (root->Color == 'W') return true;
        else return false;
    }

    void clear() override {
        delete_node(root);
        root = new Node<T>('W');
    }

    void print() const override {
        Node<T> *curr = root;
        if (curr->Color == 'W') return;
        print_node(curr);
        cout << " ";
    }

    size_t count(T num) override {
        for (auto i: *this) {
            if (i == num) return 1;
        }
        return 0;
    }

    bool contains(T num) override {
        Iterator i = this->begin();
        while (i != this->end()) {
            if (*i == num) return true;
            else ++i;
        }
        return false;
    }

    void insert (T num) override {
        Node<T> *curr = root;
        while (curr && curr->val != num) {
            if (curr->Color == 'W') {
                curr->val = num;
                curr->Color = 'B';
                size++;
                fixInsertRBTree(curr);
                return;
            }
            if (curr->val > num && curr->left == nullptr) {
                curr->left = new Node{num, 'R'};
                curr->left->parent = curr;
                size++;
                fixInsertRBTree(curr->left);
                return;
            }
            if (curr->val < num && curr->right == nullptr) {
                curr->right = new Node{num, 'R'};
                curr->right->parent = curr;
                size++;
                fixInsertRBTree(curr->right);
                return;
            }
            if (curr->val > num) {
                curr = curr->left;
            } else
                curr = curr->right;
        }

    }

    void deleteValue(T data) override {
        Node<T> *node = deleteBST(root, data);
        fixDeleteRBTree(node);
        size--;
    }

    void swap_(RBTree &copy) override {
        Node<T> *tmp = copy.root;
        copy.root = root;
        root = tmp;
    }

    [[nodiscard]] size_t length() const override {
        return size;
    }

    void make_from_array(const T *array, size_t n) override {
        for (int i = 0; i < n; i++) {
            insert(array[i]);
        }
    }

    static bool is_equal_(RBTree &curr, RBTree &ptr) {
        if (curr.length() == ptr.length()) {
            Iterator i = curr.begin();
            Iterator j = ptr.begin();
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

    RBTree &operator=(const RBTree &copy) {
        if (&copy != this) {
            if (!this->empty()) this->clear();
            make_copy(copy.root);
        }
        return *this;
    }

    friend bool operator==(RBTree &left, RBTree &right) {
        return is_equal_(left, right);
    }

    friend bool operator!=(RBTree &left, RBTree &right) {
        return left != right;
    }

    friend bool operator<(RBTree &left, RBTree &right) {
        Iterator i = left.begin();
        Iterator j = right.begin();
        while (true) {
            if (i == left.end() && j == right.end()) return false;
            if (i == left.end() && j != right.end()) return false;
            if (i != left.end() && j == right.end()) return true;
            if (*i < *j) return true;
            else {
                ++i;
                ++j;
            }
        }
    }

    friend bool operator>(RBTree &left, RBTree &right) {
        return right < left;
    }

    friend bool operator<=(RBTree &left, RBTree &right) {
        return right >= left;
    }

    friend bool operator>=(RBTree &left, RBTree &right) {
        return left >= right;
    }

    friend std::ostream &operator<<(std::ostream &left, const RBTree &right) {
        right.print_ct(left, right.root);
        return left;
    }

    friend std::istream &operator>>(std::istream &left, RBTree &right) {
        int num;
        while (left >> num) right.insert((num));
        return left;
    }
};

template <typename T>
inline abstract_data_t<T>::~abstract_data_t() = default;

int main(int argc, char const *argv[]) { //NOLINT
    int x[4] = {2, 3, 5, 7};
    RBTree <int> a(x, 4);
//    a.push(11);
    a.insert(11);
//    assert(11 == a[a.length() - 1]);
//    a.insert(a.begin(), 1);
//    assert(1 == a[0]);
//    a.extend(YOUR_VARIANT<int>({13, 17, 19}));
    a.insert(13);
    a.insert(17);
    a.insert(19);
    a.erase(a.rbegin());
    assert(a.find(17) == --a.end());
    assert(!(8 == a.length()));
    //RBTree<char> b = "abra";
    RBTree <char> b;
    b.insert('a');
    b.insert('b');
    b.insert('r');
    b.insert('a');
//    assert(0 == b.back());
    b.erase(b.rbegin());
//    b.push('c');
    b.insert('z');
    assert('z' == *b.rbegin());
    b.insert(b.end(), 'a');
    assert('a' == *b.begin());
//    b.extend(YOUR_VARIANT<char>("dabra"));
//    assert(b.rbegin() == b.find(0));
//    b[0] = 'A';
//    assert('A' == b.front());
    RBTree<std::string> c({"Hello", "world"});
//    c[0].append(",");
    c.insert(c.find("world"), " ");
//    c[c.length()].append("!");
}