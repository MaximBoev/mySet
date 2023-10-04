#include <iostream>
#include <cassert>
#include <sstream>

using namespace std;

class RBTree {
private:
    class Node {
    public:
        int val;
        char Color;
        Node *left;
        Node *right;
        Node *parent;

        explicit Node(int val, char Color) {
            this->val = val;
            this->Color = Color;
            this->left = nullptr;
            this->right = nullptr;
            this->parent = nullptr;
        }
    };
    Node *root;
    size_t size{0};
    void print_node(Node *curr)const{ // NOLINT(misc-no-recursion)
        if (!curr) return;
        print_node(curr->left);
        cout << curr->val << " ";
        print_node(curr->right);
    }
    void delete_node(Node *curr) { // NOLINT(misc-no-recursion)
        if (!curr) return;
        delete_node(curr->left);
        delete_node(curr->right);
        delete curr;
        root = nullptr;
    }
    static char getColor(Node *curr){
        if(curr == nullptr) return 'B';
        return curr->Color;
    }
    static void setColor(Node *curr, char s){
        if(curr == nullptr) return;
        curr->Color = s;
    }
    void rotateLeft(Node *ptr){
        Node *right_child = ptr->right;
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
    void rotateRight(Node *ptr){
        Node *left_child = ptr->left;
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
    void fixInsertRBTree(Node *ptr) {
        Node *parent;
        Node *grandparent;
        while (ptr != root && getColor(ptr) == 'R' && getColor(ptr->parent) == 'R') {
            parent = ptr->parent;
            grandparent = parent->parent;
            if (parent == grandparent->left) {
                Node *uncle = grandparent->right;
                if (getColor(uncle) == 'R') {
                    setColor(uncle, 'B');
                    setColor(parent, 'B');
                    setColor(grandparent, 'B');
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
                Node *uncle = grandparent->left;
                if (getColor(uncle) == 'R') {
                    setColor(uncle, 'B');
                    setColor(parent, 'B');
                    setColor(grandparent, 'B');
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
    void make_copy(Node *curr){ // NOLINT(misc-no-recursion)
        if (!curr) return;
        make_copy(curr->left);
        insert(curr->val);
        make_copy(curr->right);
    }
    void print_ct(std::ostream& l, Node *curr)const{// NOLINT(misc-no-recursion
        if (!curr) return;
        print_ct( l, curr->left);
        if(curr->Color == 'W') return;
        else l << curr->val << ' ';
        print_ct(l, curr->right);
    }
public:
    class Iterator{
    private:
        Node *id_node{};
        int max_N = 0;
        bool flag = false;
        short int lef_t = 0;
        short int righ_t = 0;
        short int paren_t = 0;
        static int max_Node(Node *curr){
            while(true){
                if(curr->right == nullptr){
                    return curr->val;
                }
                curr = curr->right;
            }
        }
        static Node *min_node(Node *curr){
            while(true){
                if(curr->left == nullptr) return curr;
                curr = curr->left;
            }
        }
    public:
        Iterator(Node *curr ,bool flag){
            if(flag){
                if(curr->Color != 'W') {
                    id_node = min_node(curr);
                    max_N = max_Node(curr);
                    lef_t = 1;
                    if (id_node->right) righ_t = 0;
                    else righ_t = 1;
                    if (id_node->parent) paren_t = 0;
                    else paren_t = 1;
                }else id_node = nullptr;
            }else id_node = nullptr;
        }
        explicit Iterator(RBTree& a){
            id_node = a.root;
            max_N = max_Node(a.root);
        }
        Iterator& operator++(){
            if(id_node->val == max_N){
                id_node = id_node->right;
                return *this;
            }
            if(lef_t == 1 && righ_t == 1 && paren_t == 1){
                id_node = id_node->right;
                return *this;
            }
            if(!righ_t){
                id_node = id_node->right;
                flag = false;
                while(id_node->left){
                    id_node = id_node->left;
                    flag = true;
                    paren_t = 0;
                }
                lef_t = 1;
                if(id_node->right) righ_t = 0;
                else righ_t = 1;
                if(!flag) { paren_t = 0;}
                else{}
                return *this;
            }else{
                Node *tmp = id_node->parent;
                if(tmp->parent) paren_t = 0;
                else paren_t = 1;
                if(id_node == tmp->left){
                    lef_t = 1;
                    if(tmp->right) righ_t = 0;
                    else righ_t = 1;
                    id_node = tmp;
                }else{
                    lef_t = 1;
                    righ_t = 1;
                    if(id_node->parent) paren_t = 0;
                    else paren_t = 1;
                    id_node = tmp;
                }
                return *this;
            }
        }
        friend int operator*(Iterator& a){
            return a.id_node->val;
        }
        friend bool operator!=(const Iterator &b, const Iterator &a){
            return b.id_node != a.id_node;
        }
        friend bool operator==(const Iterator &b, const Iterator &a){
            return b.id_node == a.id_node;
        }
    };
    Iterator begin(){return {root, true};}
    Iterator end(){return {root, false};}
    RBTree() : root(new Node{0, 'W'}) {}
    RBTree(int *arr, int n){
        root = new Node{0, 'W'};
        make_from_array(arr, n);
    }
    RBTree(RBTree& copy){
        root = new Node{0, 'W'};
        make_copy(copy.root);
    }
    ~RBTree(){delete_node(root);}
    bool empty(){
        if(root == nullptr) return true;
        if(root->Color == 'W') return true;
        else return false;
    }
    void clear(){
        delete_node(root);
        root = new Node(0, 'W');
    }
    void print() const {
        Node *curr = root;
        if(curr->Color == 'W') return;
        print_node(curr);
        cout << " ";
    }
    size_t count(int num){
        for(auto i:*this){
            if(i == num) return 1;
        }
        return 0;
    }
    bool contains(int num){
        Iterator i = this->begin();
        while (i != this->end()){
            if(*i == num) return true;
            else ++i;
        }
        return false;
    }
    void insert(int num){
        Node *curr = root;
        if(num == 0 && curr->Color == 'W'){
            curr->Color = 'R';
            size++;
            fixInsertRBTree(curr);
            return;
        }
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
    void swap_(RBTree& copy){
        Node *tmp = copy.root;
        copy.root = root;
        root = tmp;
    }
    [[nodiscard]] size_t length() const{
        return size;
    }
    void make_from_array(int *array, size_t n) {
        for (int i = 0; i < n; i++){
            insert(array[i]);
        }
    }
    static bool is_equal(RBTree& curr, RBTree& ptr){
        if(curr.length() == ptr.length()){
            Iterator i = curr.begin();
            Iterator j = ptr.begin();
            while(i!=curr.end() && j!=ptr.end()){
                if(*i != *j) return false;
                else{
                    ++i;
                    ++j;
                }
            }
            return true;
        }
        return false;
    }
    RBTree &operator=(const RBTree& copy){
        if (&copy != this){
            if(!this->empty()) this->clear();
            make_copy(copy.root);
        }
        return *this;
    }
    friend bool operator==(RBTree& left, RBTree& right){
        return is_equal(left, right);
    }
    friend bool operator!=(RBTree& left, RBTree& right){
        return !(left == right);
    }
    friend bool operator< (RBTree& left, RBTree& right){
        Iterator i = left.begin();
        Iterator j = right.begin();
        while(true){
            if(i == left.end() && j == right.end()) return false;
            if(i == left.end() && j != right.end()) return false;
            if(i != left.end() && j == right.end()) return true;
            if(*i < *j) return true;
            else{
                ++i;
                ++j;
            }
        }
    }
    friend bool operator> (RBTree& left, RBTree& right){
        return right < left;
    }
    friend bool operator<=(RBTree& left, RBTree& right){
        return !(left > right);
    }
    friend bool operator>=(RBTree& left, RBTree& right){
        return !(left < right);
    }
    friend std::ostream &operator<<(std::ostream& left, const RBTree& right){
        right.print_ct(left, right.root);
        return left;
    }
    friend std::istream &operator>>(std::istream& left, RBTree& right){
        int num;
        while (left >> num) right.insert((num));
        return left;
    }
};
typedef RBTree abstract_data_t;
int main(int argc, char const *argv[]){
    std::stringstream ss{"1 3 5 7 9"};
    abstract_data_t a;
    ss >> a;
    assert(5 == a.length());
    std::cout << a;
    abstract_data_t b(a);
    assert(a == b);
    b.insert(0);
    assert(b.contains(9));
    assert(b < a);
    assert(a > b);
    std::cout << b;
    abstract_data_t c;
    assert(0 == c.length());
    c = b;
    assert(b == c);
    std::cout << c;
    return 0;
}
