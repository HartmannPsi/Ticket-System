#pragma once
#ifndef BPT_HPP
#define BPT_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using u32 = unsigned long;

constexpr int M = 101; // M should be odd (101)
constexpr u32 BLOCK = 4096 * 2;
constexpr int CACHE_SIZE = 4096 * 2;

class exception {
protected:
  const std::string variant = "";
  std::string detail = "";

public:
  exception() {}
  exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
  virtual std::string what() { return variant + " " + detail; }
};

class index_out_of_bound : public exception {
  /* __________________________ */
};

class runtime_error : public exception {
  /* __________________________ */
};

class invalid_iterator : public exception {
  /* __________________________ */
};

class container_is_empty : public exception {
  /* __________________________ */
};

template <class T1, class T2> class pair {
public:
  T1 first;
  T2 second;
  constexpr pair() : first(), second() {}
  pair(const pair &other) = default;
  pair(pair &&other) = default;
  pair(const T1 &x, const T2 &y) : first(x), second(y) {}
  template <class U1, class U2> pair(U1 &&x, U2 &&y) : first(x), second(y) {}
  template <class U1, class U2>
  pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
  template <class U1, class U2>
  pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

template <class Key, class T, class Compare = std::less<Key>>
class map { // AVL tree
public:
  typedef pair<const Key, T> value_type;
  class iterator;
  class const_iterator;

private:
  struct Node;

  bool le(const Key &lhs, const Key &rhs) const { return cmp(lhs, rhs); }

  bool gr(const Key &lhs, const Key &rhs) const { return cmp(rhs, lhs); }

  bool eq(const Key &lhs, const Key &rhs) const {
    return !(cmp(lhs, rhs) || cmp(rhs, lhs));
  }

  bool ieq(const Key &lhs, const Key &rhs) const {
    return cmp(lhs, rhs) || cmp(rhs, lhs);
  }

  template <class A, class B>
  static pair<A, B> &putv(pair<A, B> &lhs, const pair<A, B> &rhs) {
    char *_lhs = reinterpret_cast<char *>(&lhs);
    const char *_rhs = reinterpret_cast<const char *>(&rhs);

    const int size = sizeof(pair<A, B>) / sizeof(char);

    for (int i = 0; i != size; ++i) {
      _lhs[i] = _rhs[i];
    }
    return lhs;
  }

  struct Node {
    value_type *data = nullptr;
    Node *left;
    Node *right;
    Node *parent;
    int height;

    Node(const value_type &element, Node *pt = nullptr,
         Node *const lt = nullptr, Node *const rt = nullptr, int h = 1)
        : data(new value_type(element)), parent(pt), left(lt), right(rt),
          height(h) {}

    ~Node() {
      delete data;
      data = nullptr;
    }
  };

  Node *root;
  size_t _size;
  Node *beg;
  Compare cmp;

  Node *find_beg() {
    if (root == nullptr) {
      return nullptr;
    }

    Node *tmp = root;
    while (tmp->left != nullptr) {
      tmp = tmp->left;
    }
    return tmp;
  }

  iterator insert(const value_type &val, Node *&pos, Node *pt = nullptr) {
    auto ans = end();

    if (pos == nullptr) {
      pos = new Node(val);
      pos->parent = pt;
      ans = iterator(pos, this);
      ++_size;

      if (beg == nullptr || le(val.first, beg->data->first)) {
        beg = pos;
      }

    } else if (le(val.first, pos->data->first)) {
      ans = insert(val, pos->left, pos);
      if (height(pos->left) - height(pos->right) == 2) {
        if (le(val.first, pos->left->data->first)) {
          LL(pos);
        } else {
          LR(pos);
        }
      }

    } else {
      ans = insert(val, pos->right, pos);
      if (height(pos->right) - height(pos->left) == 2) {
        if (le(pos->right->data->first, val.first)) {
          RR(pos);
        } else {
          RL(pos);
        }
      }
    }

    pos->height = std::max(height(pos->left), height(pos->right)) + 1;
    return ans;
  }

  bool erase(const Key &_key, Node *&pos) {
    if (pos == nullptr) {
      return true;
    }

    if (eq(_key, pos->data->first)) {
      if (pos->left == nullptr || pos->right == nullptr) {
        Node *tmp = pos;
        pos = (pos->left != nullptr) ? pos->left : pos->right;
        if (pos != nullptr) {
          pos->parent = tmp->parent;
        }
        --_size;

        if (beg == tmp) {
          if (pos == nullptr) {
            beg = tmp->parent;
          } else {
            beg = pos;
          }
        }

        delete tmp;
        return false;
      } else {
        Node **tmpp = &(pos->right);

        while ((*tmpp)->left != nullptr) {
          tmpp = &((*tmpp)->left);
        }
        Node *&tmp = *tmpp;
        Node *const _tmp = tmp;
        Node *const _pos = pos;
        const auto pos_h = pos->height;
        const auto tmp_h = tmp->height;

        if (tmp != pos->right) {

          Node *const pos_lt = pos->left;
          Node *const pos_rt = pos->right;
          Node *const pos_pt = pos->parent;
          Node *const tmp_lt = tmp->left;
          Node *const tmp_rt = tmp->right;
          Node *const tmp_pt = tmp->parent;
          tmp->left = pos_lt;
          tmp->right = pos_rt;
          tmp->parent = pos_pt;
          tmp->height = pos_h;
          pos->left = tmp_lt;
          pos->right = tmp_rt;
          pos->parent = tmp_pt;
          pos->height = tmp_h;

          pos_lt->parent = _tmp;
          pos_rt->parent = _tmp;
          if (tmp_rt != nullptr) {
            tmp_rt->parent = _pos;
          }

          pos = _tmp;
          tmp = _pos;

          delete tmp->data;
          tmp->data = new value_type(*(pos->data));
        } else {
          pos = _tmp;
          pos->left = _pos->left;
          _pos->left = nullptr;
          _pos->right = pos->right;
          pos->right = _pos;
          pos->height = pos_h;
          _pos->height = tmp_h;

          pos->left->parent = pos;
          pos->parent = _pos->parent;
          pos->right->parent = pos;
          if (_pos->right != nullptr) {
            _pos->right->parent = _pos;
          }

          delete _pos->data;
          _pos->data = new value_type(*(pos->data));
        }

        if (erase(pos->data->first, pos->right)) {
          return true;
        } else {
          return adjust(pos, true);
        }
      }
    } else if (le(_key, pos->data->first)) {
      if (erase(_key, pos->left)) {
        return true;
      } else {
        return adjust(pos, false);
      }
    } else {
      if (erase(_key, pos->right)) {
        return true;
      } else {
        return adjust(pos, true);
      }
    }
  }

  bool adjust(Node *&pos, bool sub_tree) {
    const int h_pos_ll = (pos->left == nullptr) ? height(pos->left)
                                                : height(pos->left->left),
              h_pos_lr = (pos->left == nullptr) ? height(pos->left)
                                                : height(pos->left->right),
              h_pos_rl = (pos->right == nullptr) ? height(pos->right)
                                                 : height(pos->right->left),
              h_pos_rr = (pos->right == nullptr) ? height(pos->right)
                                                 : height(pos->right->right);

    if (sub_tree) { // right_subtree
      if (height(pos->left) - height(pos->right) == 1) {
        return true;

      } else if (height(pos->left) == height(pos->right)) {
        --(pos->height);
        return false;

      } else if (h_pos_lr > h_pos_ll) {
        LR(pos);
        return false;

      } else {
        LL(pos);
        return height(pos->right) != height(pos->left);
      }

    } else { // left_subtree
      if (height(pos->right) - height(pos->left) == 1) {
        return true;

      } else if (height(pos->right) == height(pos->left)) {
        --(pos->height);
        return false;

      } else if (h_pos_rl > h_pos_rr) {
        RL(pos);
        return false;

      } else {
        RR(pos);
        return height(pos->left) != height(pos->right);
      }
    }
  }

  void clear(Node *&pos) {
    if (pos == nullptr) {
      return;
    }

    clear(pos->left);
    clear(pos->right);

    delete pos;
    pos = nullptr;
  }

  int height(Node *pos) const { return pos == nullptr ? 0 : pos->height; }

  void LL(Node *&pos) {
    if (pos->left == nullptr) {
      return;
    }
    Node *tmp = pos->left;
    pos->left = tmp->right;
    tmp->right = pos;
    tmp->parent = pos->parent;
    pos->parent = tmp;
    if (pos->left != nullptr) {
      pos->left->parent = pos;
    }

    pos->height = std::max(height(pos->left), height(pos->right)) + 1;
    tmp->height = std::max(height(tmp->left), height(pos)) + 1;
    pos = tmp;
  }

  void RR(Node *&pos) {
    if (pos->right == nullptr) {
      return;
    }
    Node *tmp = pos->right;
    pos->right = tmp->left;
    tmp->left = pos;
    tmp->parent = pos->parent;
    pos->parent = tmp;
    if (pos->right != nullptr) {
      pos->right->parent = pos;
    }

    pos->height = std::max(height(pos->right), height(pos->left)) + 1;
    tmp->height = std::max(height(tmp->right), height(pos)) + 1;
    pos = tmp;
  }

  void LR(Node *&pos) {
    RR(pos->left);
    LL(pos);
  }

  void RL(Node *&pos) {
    LL(pos->right);
    RR(pos);
  }

  Node *clone_tree(const Node *pos, Node *const pt = nullptr) {
    if (pos == nullptr) {
      return nullptr;
    }

    Node *new_node = new Node(*(pos->data));

    new_node->left = clone_tree(pos->left, new_node);
    new_node->right = clone_tree(pos->right, new_node);
    new_node->parent = pt;

    return new_node;
  }

public:
  class iterator {
  private:
    friend class map;
    Node *ptr;
    map<Key, T, Compare> *father;

    iterator(Node *_ptr, map<Key, T, Compare> *_ftr)
        : ptr(_ptr), father(_ftr) {}

  public:
    iterator() : ptr(nullptr), father(nullptr) {}

    iterator(const iterator &other)
        : ptr(const_cast<Node *>(other.ptr)),
          father(const_cast<map<Key, T, Compare> *>(other.father)) {}

    ~iterator() {}

    iterator &operator=(const iterator &other) {
      if (&other == this) {
        return *this;
      }
      ptr = other.ptr;
      father = other.father;

      return *this;
    }

    iterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator &operator++() {
      if (ptr == nullptr) {
        throw invalid_iterator();
      }

      if (ptr->right != nullptr) {
        ptr = ptr->right;
        while (ptr->left != nullptr) {
          ptr = ptr->left;
        }
        return *this;

      } else {

        while (true) {
          const Node *tmp = ptr;
          ptr = ptr->parent;
          if (ptr == nullptr || ptr->left == tmp) {
            break;
          }
        }
        return *this;
      }
    }

    iterator operator--(int) {
      auto tmp = *this;
      --(*this);
      return tmp;
    }

    iterator &operator--() {
      if (*this == father->begin()) {
        throw invalid_iterator();

      } else if (ptr == nullptr) {
        ptr = father->root;
        while (ptr->right != nullptr) {
          ptr = ptr->right;
        }
        return *this;
      }

      if (ptr->left != nullptr) {
        ptr = ptr->left;
        while (ptr->right != nullptr) {
          ptr = ptr->right;
        }
        return *this;

      } else {

        while (true) {
          const Node *tmp = ptr;
          ptr = ptr->parent;
          if (ptr == nullptr || ptr->right == tmp) {
            break;
          }
        }
        return *this;
      }
    }

    value_type &operator*() const {

      if (ptr == nullptr) {
        throw invalid_iterator();
      }

      return *(ptr->data);
    }

    bool operator==(const iterator &rhs) const {

      return father == rhs.father && ptr == rhs.ptr;
    }

    bool operator==(const const_iterator &rhs) const {

      return father == rhs.father && ptr == rhs.ptr;
    }

    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }

    value_type *operator->() const noexcept { return ptr->data; }
  };
  class const_iterator {
  private:
    friend class map;
    const Node *ptr;
    const map<Key, T, Compare> *father;

    const_iterator(const Node *_ptr, const map<Key, T, Compare> *_ftr)
        : ptr(_ptr), father(_ftr) {}

  public:
    const_iterator() : ptr(nullptr), father(nullptr) {}

    const_iterator(const const_iterator &other)
        : ptr(other.ptr), father(other.father) {}

    const_iterator(const iterator &other)
        : ptr(other.ptr), father(other.father) {}

    const_iterator &operator=(const const_iterator &other) {
      if (&other == this) {
        return *this;
      }
      ptr = other.ptr;
      father = other.father;
      return *this;
    }

    const_iterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    const_iterator &operator++() {
      if (ptr == nullptr) {
        throw invalid_iterator();
      }

      if (ptr->right != nullptr) {
        ptr = ptr->right;
        while (ptr->left != nullptr) {
          ptr = ptr->left;
        }
        return *this;

      } else {

        while (true) {
          const Node *tmp = ptr;
          ptr = ptr->parent;
          if (ptr == nullptr || ptr->left == tmp) {
            break;
          }
        }
        return *this;
      }
    }

    const_iterator operator--(int) {
      auto tmp = *this;
      --(*this);
      return tmp;
    }

    const_iterator &operator--() {
      if (*this == father->cbegin()) {
        throw invalid_iterator();
      } else if (ptr == nullptr) {
        ptr = father->root;
        while (ptr->right != nullptr) {
          ptr = ptr->right;
        }
        return *this;
      }

      if (ptr->left != nullptr) {
        ptr = ptr->left;
        while (ptr->right != nullptr) {
          ptr = ptr->right;
        }
        return *this;

      } else {

        while (true) {
          const Node *tmp = ptr;
          ptr = ptr->parent;
          if (ptr == nullptr || ptr->right == tmp) {
            break;
          }
        }
        return *this;
      }
    }

    const value_type &operator*() const {
      if (ptr == nullptr) {
        throw invalid_iterator();
      }
      return *(ptr->data);
    }

    bool operator==(const const_iterator &rhs) const {
      return father == rhs.father && ptr == rhs.ptr;
    }

    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }

    const value_type *operator->() const noexcept { return ptr->data; }
  };

  map() : root(nullptr), _size(0), beg(nullptr) {}
  map(const map &other) : _size(other._size) {
    root = clone_tree(other.root);
    beg = find_beg();
  }

  map &operator=(const map &other) {
    if (&other == this) {
      return *this;
    }

    clear(root);
    _size = other._size;
    root = clone_tree(other.root);
    beg = find_beg();

    return *this;
  }

  ~map() {
    clear(root);
    _size = 0;
  }

  T &at(const Key &key) {
    Node *pos = root;

    while (pos != nullptr && ieq(pos->data->first, key)) {
      if (gr(pos->data->first, key)) {
        pos = pos->left;
      } else {
        pos = pos->right;
      }
    }

    if (pos == nullptr) {
      throw index_out_of_bound();
    } else {
      return pos->data->second;
    }
  }

  const T &at(const Key &key) const {
    const Node *pos = root;

    while (pos != nullptr && ieq(pos->data->first, key)) {
      if (gr(pos->data->first, key)) {
        pos = pos->left;
      } else {
        pos = pos->right;
      }
    }

    if (pos == nullptr) {
      throw index_out_of_bound();
    } else {
      return pos->data->second;
    }
  }

  T &operator[](const Key &key) {
    if (find(key) == cend()) {
      pair<iterator, bool> ans = insert(value_type(key, T()));
      return ans.first.ptr->data->second;
    } else {
      return at(key);
    }
  }

  const T &operator[](const Key &key) const { return at(key); }

  iterator begin() { return iterator(beg, this); }

  const_iterator cbegin() const { return const_iterator(beg, this); }

  iterator end() { return iterator(nullptr, this); }

  const_iterator cend() const { return const_iterator(nullptr, this); }

  bool empty() const { return _size == 0; }

  size_t size() const { return _size; }

  void clear() {
    clear(root);
    _size = 0;
    beg = nullptr;
  }

  pair<iterator, bool> insert(const value_type &value) {
    auto res = find(value.first);
    if (res != cend()) {
      return {res, false};
    }
    return {insert(value, root), true};
  }

  void erase(iterator pos) {
    if (pos == end() || pos.father != this) {
      throw invalid_iterator();
    }
    if (_size == 0) {
      throw container_is_empty();
    }
    const Key _key = pos.ptr->data->first;
    erase(_key, root);
  }

  size_t count(const Key &key) const {
    if (find(key) == cend()) {
      return 0;
    } else {
      return 1;
    }
  }

  iterator find(const Key &key) {
    Node *pos = root;

    while (pos != nullptr && ieq(pos->data->first, key)) {
      if (gr(pos->data->first, key)) {
        pos = pos->left;
      } else {
        pos = pos->right;
      }
    }

    if (pos == nullptr) {
      return end();
    } else {
      return iterator(pos, this);
    }
  }

  const_iterator find(const Key &key) const {
    const Node *pos = root;

    while (pos != nullptr && ieq(pos->data->first, key)) {
      if (gr(pos->data->first, key)) {
        pos = pos->left;
      } else {
        pos = pos->right;
      }
    }

    if (pos == nullptr) {
      return cend();
    } else {
      return const_iterator(pos, this);
    }
  }
};

template <typename T1, typename T2> struct Pair {
  T1 first;
  T2 second;

  Pair() {}

  Pair(const Pair &other) : first(other.first), second(other.second) {}

  Pair(const T1 &a, const T2 &b) : first(a), second(b) {}

  ~Pair() {}

  Pair &operator=(const Pair &other) {
    if (&other == this) {
      return *this;
    }

    first = other.first;
    second = other.second;
    return *this;
  }
};

template <typename T> class List {

public:
  struct _Node {
    T val;
    _Node *next = nullptr;
    _Node *prev = nullptr;

    _Node() {}
    _Node(const T &_val) : val(_val) {}
  };
  _Node *head;
  _Node *end;
  int s = 0;

public:
  _Node *push_front(const T &val) {
    _Node *new_node = new _Node(val);
    new_node->next = head->next;
    if (new_node->next != nullptr) {
      new_node->next->prev = new_node;
    }
    head->next = new_node;
    new_node->prev = head;
    ++s;
    return new_node;
  }

  void pop_front() {

    if (s == 0) {
      throw 0;
      return;
    }
    auto tmp = head->next;
    head->next = head->next->next;
    if (head->next != nullptr) {
      head->next->prev = head;
    }
    delete tmp;
    --s;
  }

  void pop_back() {

    if (s == 0) {
      throw 0;
      return;
    }
    auto tmp = end->prev;
    end->prev = end->prev->prev;
    if (end->prev != nullptr) {
      end->prev->next = end;
    }
    delete tmp;
    --s;
  }

  void update_to_front(_Node *pos) {
    if (pos->prev == head) {
      return;
    }
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    pos->prev = head;
    pos->next = head->next;
    head->next->prev = pos;
    head->next = pos;
  }

  void erase(_Node *pos) {
    pos->next->prev = pos->prev;
    pos->prev->next = pos->next;
    delete pos;
    --s;
  }

  int size() const { return s; }

  bool empty() const { return s == 0; }

  void clear() {
    _Node *pos = head->next;
    while (pos != end) {
      auto tmp = pos;
      pos = pos->next;
      delete tmp;
    }
    s = 0;
  }

  T &top() { return head->next->val; }

  T &back() { return end->prev->val; }

  List() {
    head = new _Node;
    end = new _Node;
    head->next = end;
    end->prev = head;
  }

  ~List() {
    clear();
    delete head;
    delete end;
  }
};

template <typename Key> class BPlusTree {

  class LRUCache;
  friend class BPlusTree::LRUCache;

  struct Node {
    Key index[M];
    u32 child[M] = {};
    int size = 0; // count of indexes, size <= M - 1
    bool is_leaf = false;
    u32 next = INT32_MAX;

    Node() {}
    ~Node() {}

    Node &operator=(const Node &other) {
      if (&other == this) {
        return *this;
      }

      is_leaf = other.is_leaf;
      next = other.next;
      size = other.size;
      for (int i = 0; i != size; ++i) {
        index[i] = other.index[i];
      }
      for (int i = 0; i <= size; ++i) {
        child[i] = other.child[i];
      }

      return *this;
    }
  };

  std::fstream ind_file, ind_rec, _root;
  const std::string ind;
  List<u32> ind_pool;
  u32 root;

  // read sth from the files
  void read_node(Node &dest, u32 pos);

  // write sth to the files
  void write_node(const Node &src, u32 pos);

  // return a position to be used from the recycle pools
  u32 free_node();

  void read(Node &dest, u32 pos) {

    ind_file.close();
    ind_file.open(ind, std::ios::in | std::ios::out);
    ind_file.seekg(pos);
    ind_file.read(reinterpret_cast<char *>(&dest), sizeof(Node));
  }

  void write(const Node &src, u32 pos) {

    ind_file.close();
    ind_file.open(ind, std::ios::in | std::ios::out);
    ind_file.seekp(pos);
    ind_file.write(reinterpret_cast<const char *>(&src), sizeof(Node));
  }

  // add a position to the recycle pools
  void rec_node(u32 pos);

  void insert_internal(Node node[], int i, int child[], u32 pos[], u32 new_pos,
                       const Key &new_ind);

  void merge_internal(Node node[], int i, int child[], u32 pos[]);

  struct LRUCache {
    List<Pair<Node, u32>> list;
    map<u32, typename List<Pair<Node, u32>>::_Node *> table;
    BPlusTree<Key> *pt;

  public:
    LRUCache(BPlusTree<Key> *_pt) : pt(_pt) {}

    ~LRUCache() { clear(); }

    Node *find(u32 pos) {
      auto res = table.find(pos);

      if (res == table.end()) {
        return add(pos);
      } else {
        list.update_to_front(res->second);
        return &(res->second->val.first);
      }
    }

    Node *add(u32 pos) {
      Node new_node;
      pt->read(new_node, pos);

      auto tmp = list.push_front({new_node, pos});
      table.insert({pos, tmp});

      if (list.size() > CACHE_SIZE) {
        pop();
      }

      return &(tmp->val.first);
    }

    void pop() {
      const auto back = list.back();

      pt->write(back.first, back.second);

      auto it = table.find(back.second);

      table.erase(it);
      list.pop_back();
    }

    void erase(u32 pos) {
      auto res = table.find(pos);
      if (res == table.end()) {
        return;
      }

      list.erase(res->second);
      table.erase(res);
    }

    void modify(u32 pos, const Node &src) {
      auto tmp = find(pos);
      *tmp = src;
    }

    void clear() {
      while (!list.empty()) {
        pop();
      }
    }
  } cache;

public:
  BPlusTree(const std::string &index_file = "index.dat",
            const std::string &index_recycle = "index.rec",
            const std::string &r = ".root");

  ~BPlusTree();

  void insert(const Key &ind);

  void erase(const Key &ind);

  void find(const Key &ind);

  void traverse();

  int count(const Key &ind);
};

template <typename Key>
int lower_bound(const Key *const beg, int size, const Key &index);

template <typename Key>
int upper_bound(const Key *const beg, int size, const Key &index);

template <typename Key>
BPlusTree<Key>::BPlusTree(const std::string &index_file,

                          const std::string &index_recycle,

                          const std::string &r)
    : cache(this), ind(index_file) {

  ind_file.open(index_file, std::ios::in | std::ios::out);
  if (!ind_file.is_open()) { // 1st time to open
    ind_file.open(index_file, std::ios::out);
    ind_file.close();
    ind_file.open(index_file, std::ios::in | std::ios::out);

    ind_rec.open(index_recycle, std::ios::out);
    _root.open(r, std::ios::out);

    ind_pool.push_front(0 + BLOCK);
    root = 0;
    Node node;
    node.is_leaf = true;
    node.next = INT32_MAX;

    ind_file.seekp(root);
    ind_file.write(reinterpret_cast<const char *>(&node), sizeof(Node));

  } else { // restore the recycle pool
    ind_rec.open(index_recycle, std::ios::in);
    _root.open(r, std::ios::in);

    _root.seekg(0);
    _root.read(reinterpret_cast<char *>(&root), sizeof(u32));

    ind_rec.seekg(std::ios::end);
    const int ind_end = ind_rec.tellg();
    for (int i = 0; i < ind_end; i += sizeof(u32)) {
      ind_rec.seekg(i);
      u32 pos;
      ind_rec.read(reinterpret_cast<char *>(&pos), sizeof(u32));
      ind_pool.push_front(pos);
    }

    ind_rec.close();
    _root.close();
    ind_rec.open(index_recycle, std::ios::out);
    _root.open(r, std::ios::out);
  }

  cache.add(root);
}

template <typename Key> BPlusTree<Key>::~BPlusTree() {

  // restore cache to file
  cache.clear();

  // save the pos of root
  _root.seekp(0);
  _root.write(reinterpret_cast<const char *>(&root), sizeof(u32));

  // save the recycle pools
  u32 i = 0;
  for (; !ind_pool.empty(); ind_pool.pop_front()) {
    const u32 &tmp = ind_pool.top();
    ind_rec.seekp(i);
    ind_rec.write(reinterpret_cast<const char *>(&tmp), sizeof(u32));
  }

  ind_file.close();
  ind_rec.close();
  _root.close();
}

template <typename Key> void BPlusTree<Key>::read_node(Node &dest, u32 pos) {
  auto read = cache.find(pos);
  dest = *read;
}

template <typename Key>
void BPlusTree<Key>::write_node(const Node &src, u32 pos) {
  cache.modify(pos, src);
}

template <typename Key> u32 BPlusTree<Key>::free_node() {
  u32 res = ind_pool.top();
  ind_pool.pop_front();
  if (ind_pool.empty()) {
    ind_pool.push_front(res + BLOCK);
  }

  return res;
}

template <typename Key> void BPlusTree<Key>::rec_node(u32 pos) {
  cache.erase(pos);
  ind_pool.push_front(pos);
}

template <typename Key> void BPlusTree<Key>::insert(const Key &ind) {
  Node node[8];
  u32 pos[8];
  int child[8];
  pos[0] = root;
  child[0] = -1;
  int i = 0;
  read_node(node[0], root);

  while (!node[i].is_leaf) {
    ++i;
    if (ind > node[i - 1].index[node[i - 1].size - 1]) {
      pos[i] = node[i - 1].child[node[i - 1].size];
      child[i] = node[i - 1].size;
      read_node(node[i], node[i - 1].child[node[i - 1].size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int _pos = lower_bound(node[i - 1].index, node[i - 1].size, ind);
      pos[i] = node[i - 1].child[_pos];
      child[i] = _pos;
      read_node(node[i], node[i - 1].child[_pos]);
    }
  }

  if (node[i].size == 0) { // root
    node[i].index[0] = ind;
    ++node[i].size;
    write_node(node[i], pos[i]);
    return;

  } else if (node[i].size < M - 1) { // trivial
    if (node[i].index[node[i].size - 1] <= ind) {
      node[i].index[node[i].size] = ind;
      ++node[i].size;
    } else {
      int _pos = upper_bound(node[i].index, node[i].size, ind);
      for (int j = node[i].size; j > _pos; --j) {
        node[i].index[j] = node[i].index[j - 1];
      }
      node[i].index[_pos] = ind;
      ++node[i].size;
    }
    write_node(node[i], pos[i]);
    return;
  }
  // split leaves

  if (node[i].index[node[i].size - 1] <= ind) {
    node[i].index[node[i].size] = ind;

  } else {
    int _pos = upper_bound(node[i].index, node[i].size, ind);
    for (int j = node[i].size; j > _pos; --j) {
      node[i].index[j] = node[i].index[j - 1];
    }
    node[i].index[_pos] = ind;
  }

  node[i].size = M / 2;
  Node new_leaf;
  u32 new_node_pos = free_node();
  new_leaf.is_leaf = true;
  new_leaf.size = 1 + M / 2; // M odd
  new_leaf.next = node[i].next;
  node[i].next = new_node_pos;
  for (int j = 0; j != 1 + M / 2; ++j) {
    new_leaf.index[j] = node[i].index[M / 2 + j];
  }
  write_node(new_leaf, new_node_pos);
  write_node(node[i], pos[i]);

  insert_internal(node, i - 1, child, pos, new_node_pos,
                  node[i].index[node[i].size - 1]);
}

template <typename Key>
void BPlusTree<Key>::insert_internal(Node node[], int i, int child[], u32 pos[],
                                     u32 new_pos, const Key &new_ind) {
  if (i == -1) { // split root
    Node new_root;
    u32 new_root_pos = free_node();
    new_root.size = 1;
    new_root.child[0] = pos[0];
    new_root.child[1] = new_pos;
    new_root.index[0] = new_ind;
    write_node(new_root, new_root_pos);
    root = new_root_pos;
    return;
  }

  if (node[i].size < M - 1) { // no splitting
    int _pos = child[i + 1];
    for (int j = node[i].size + 1; j > _pos; --j) {
      node[i].child[j] = node[i].child[j - 1];
    }
    node[i].child[_pos + 1] = new_pos;
    for (int j = node[i].size; j > _pos; --j) {
      node[i].index[j] = node[i].index[j - 1];
    }
    node[i].index[_pos] = new_ind;
    ++node[i].size;
    write_node(node[i], pos[i]);
    return;
  }

  // split this node
  u32 tmp_child[M + 1];
  Node new_node;
  u32 new_node_pos = free_node();
  new_node.size = M / 2;
  node[i].size = M / 2;
  for (int j = 0; j != M; ++j) {
    tmp_child[j] = node[i].child[j];
  }

  int _pos = child[i + 1];
  for (int j = M; j > _pos; --j) {
    tmp_child[j] = tmp_child[j - 1];
  }
  tmp_child[_pos + 1] = new_pos;
  for (int j = M - 1; j > _pos; --j) {
    node[i].index[j] = node[i].index[j - 1];
  }
  node[i].index[_pos] = new_ind;

  for (int j = 0; j != 1 + M / 2; ++j) { // M odd
    new_node.child[j] = tmp_child[j + 1 + M / 2];
    node[i].child[j] = tmp_child[j];
  }
  for (int j = 0; j != M / 2; ++j) {
    new_node.index[j] = node[i].index[j + 1 + M / 2];
  }

  write_node(node[i], pos[i]);
  write_node(new_node, new_node_pos);
  insert_internal(node, i - 1, child, pos, new_node_pos, node[i].index[M / 2]);
}

template <typename Key> int BPlusTree<Key>::count(const Key &ind) {
  Node node[8];
  u32 pos[8];
  int child[8];
  pos[0] = root;
  child[0] = -1;
  int i = 0;
  read_node(node[0], root);

  while (!node[i].is_leaf) {
    ++i;
    if (ind > node[i - 1].index[node[i - 1].size - 1]) {
      pos[i] = node[i - 1].child[node[i - 1].size];
      child[i] = node[i - 1].size;
      read_node(node[i], node[i - 1].child[node[i - 1].size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int _pos = lower_bound(node[i - 1].index, node[i - 1].size, ind);
      pos[i] = node[i - 1].child[_pos];
      child[i] = _pos;
      read_node(node[i], node[i - 1].child[_pos]);
    }
  }

  if (i == 0 && node[i].size == 0) {
    return 0;
  }

  if (ind > node[i].index[node[i].size - 1]) {
    return 0;
  }
  int _pos = lower_bound(node[i].index, node[i].size, ind);

  if (node[i].index[_pos] != ind) {
    return 0;
  }

  return 1;
}

template <typename Key> void BPlusTree<Key>::erase(const Key &ind) {
  Node node[8];
  u32 pos[8];
  int child[8];
  pos[0] = root;
  child[0] = -1;
  int i = 0;
  read_node(node[0], root);

  while (!node[i].is_leaf) {
    ++i;
    if (ind > node[i - 1].index[node[i - 1].size - 1]) {
      pos[i] = node[i - 1].child[node[i - 1].size];
      child[i] = node[i - 1].size;
      read_node(node[i], node[i - 1].child[node[i - 1].size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int _pos = lower_bound(node[i - 1].index, node[i - 1].size, ind);
      pos[i] = node[i - 1].child[_pos];
      child[i] = _pos;
      read_node(node[i], node[i - 1].child[_pos]);
    }
  }

  if (i == 0 && node[i].size == 0) {
    return;
  }

  if (ind > node[i].index[node[i].size - 1]) {
    return;
  }
  int _pos = lower_bound(node[i].index, node[i].size, ind);

  if (node[i].index[_pos] != ind) {
    return;
  }

  for (int j = _pos; j != node[i].size - 1; ++j) {
    node[i].index[j] = node[i].index[j + 1];
  }
  --node[i].size;

  if (node[i].size >= M / 2 || i == 0) { // node == root OR node size > 50
    write_node(node[i], pos[i]);
    return;
  }

  if (child[i] == 0) { // the first son of the father, only right brother

    Node rt_bro;
    read_node(rt_bro, node[i - 1].child[1]);
    if (rt_bro.size > M / 2) { // borrow from right brother
      node[i].index[node[i].size] = rt_bro.index[0];
      node[i].size = M / 2;
      --rt_bro.size;
      for (int j = 0; j != rt_bro.size; ++j) {
        rt_bro.index[j] = rt_bro.index[j + 1];
      }
      node[i - 1].index[0] = node[i].index[node[i].size - 1];

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(rt_bro, node[i - 1].child[1]);
      return;

    } else { // merge with right brother
      rec_node(node[i - 1].child[1]);

      for (int j = 0; j != rt_bro.size; ++j) {
        node[i].index[j + node[i].size] = rt_bro.index[j];
      }
      node[i].size += rt_bro.size;

      for (int j = 0; j != node[i - 1].size - 1; ++j) {
        node[i - 1].index[j] = node[i - 1].index[j + 1];
      }
      for (int j = 1; j != node[i - 1].size; ++j) {
        node[i - 1].child[j] = node[i - 1].child[j + 1];
      }
      --node[i - 1].size;
      node[i].next = rt_bro.next;
      write_node(node[i], pos[i]);

      // merge internal nodes
      merge_internal(node, i - 1, child, pos);
    }

  } else if (child[i] ==
             node[i - 1]
                 .size) { // the last son of ther father, only left brother

    Node lt_bro;
    read_node(lt_bro, node[i - 1].child[node[i - 1].size - 1]);
    if (lt_bro.size > M / 2) { // borrow from left brother
      for (int j = node[i].size; j > 0; --j) {
        node[i].index[j] = node[i].index[j - 1];
      }
      node[i].size = M / 2;
      node[i].index[0] = lt_bro.index[lt_bro.size - 1];
      --lt_bro.size;
      node[i - 1].index[node[i - 1].size - 1] = lt_bro.index[lt_bro.size - 1];

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(lt_bro, node[i - 1].child[node[i - 1].size - 1]);
      return;

    } else { // merge with left brother
      rec_node(pos[i]);
      for (int j = 0; j != node[i].size; ++j) {
        lt_bro.index[j + lt_bro.size] = node[i].index[j];
      }
      lt_bro.size += node[i].size;
      --node[i - 1].size;
      lt_bro.next = node[i].next;
      write_node(lt_bro, node[i - 1].child[node[i - 1].size]);

      // merge internal nodes
      merge_internal(node, i - 1, child, pos);
    }

  } else { // have 2 brothers

    Node lt_bro, rt_bro;
    read_node(rt_bro, node[i - 1].child[child[i] + 1]);

    if (rt_bro.size > M / 2) { // borrow from right brother

      node[i].index[node[i].size] = rt_bro.index[0];
      node[i].size = M / 2;
      --rt_bro.size;

      for (int j = 0; j != rt_bro.size; ++j) {
        rt_bro.index[j] = rt_bro.index[j + 1];
      }

      node[i - 1].index[child[i]] = node[i].index[node[i].size - 1];

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(rt_bro, node[i - 1].child[child[i] + 1]);
      return;
    }

    read_node(lt_bro, node[i - 1].child[child[i] - 1]);
    if (lt_bro.size > M / 2) { // borrow from left brother

      for (int j = node[i].size; j > 0; --j) {
        node[i].index[j] = node[i].index[j - 1];
      }
      node[i].size = M / 2;
      node[i].index[0] = lt_bro.index[lt_bro.size - 1];
      --lt_bro.size;
      node[i - 1].index[child[i] - 1] = lt_bro.index[lt_bro.size - 1];

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(lt_bro, node[i - 1].child[child[i] - 1]);
      return;
    }

    // merge with right brother
    rec_node(node[i - 1].child[child[i] + 1]);
    for (int j = 0; j != rt_bro.size; ++j) {
      node[i].index[j + node[i].size] = rt_bro.index[j];
    }
    node[i].size += rt_bro.size;

    for (int j = child[i]; j != node[i - 1].size - 1; ++j) {
      node[i - 1].index[j] = node[i - 1].index[j + 1];
    }
    for (int j = child[i] + 1; j != node[i - 1].size; ++j) {
      node[i - 1].child[j] = node[i - 1].child[j + 1];
    }
    --node[i - 1].size;
    node[i].next = rt_bro.next;
    write_node(node[i], pos[i]);

    // merge internal nodes
    merge_internal(node, i - 1, child, pos);
  }
}

template <typename Key>
void BPlusTree<Key>::merge_internal(Node node[], int i, int child[],
                                    u32 pos[]) {

  if (i == 0) {
    if (node[0].size > 0) {
      write_node(node[0], pos[0]);
      return;
    } else {
      root = node[0].child[0];
      rec_node(pos[0]);
      return;
    }
  }

  if (node[i].size >= M / 2) {
    write_node(node[i], pos[i]);
    return;
  }

  if (child[i] == 0) { // only right brother

    Node rt_bro;
    read_node(rt_bro, node[i - 1].child[1]);
    if (rt_bro.size > M / 2) { // borrow from right brother
      node[i].index[node[i].size] = node[i - 1].index[0];
      node[i].child[node[i].size + 1] = rt_bro.child[0];
      node[i - 1].index[0] = rt_bro.index[0];
      node[i].size = M / 2;
      --rt_bro.size;

      for (int j = 0; j != rt_bro.size; ++j) {
        rt_bro.index[j] = rt_bro.index[j + 1];
      }
      for (int j = 0; j <= rt_bro.size; ++j) {
        rt_bro.child[j] = rt_bro.child[j + 1];
      }

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(rt_bro, node[i - 1].child[1]);
      return;

    } else { // merge with right brother
      rec_node(node[i - 1].child[1]);

      node[i].index[node[i].size] = node[i - 1].index[0];
      for (int j = 0; j != rt_bro.size; ++j) {
        node[i].index[j + 1 + node[i].size] = rt_bro.index[j];
      }
      for (int j = 0; j <= rt_bro.size; ++j) {
        node[i].child[j + 1 + node[i].size] = rt_bro.child[j];
      }
      node[i].size += rt_bro.size + 1;

      for (int j = 0; j != node[i - 1].size - 1; ++j) {
        node[i - 1].index[j] = node[i - 1].index[j + 1];
      }
      for (int j = 1; j != node[i - 1].size; ++j) {
        node[i - 1].child[j] = node[i - 1].child[j + 1];
      }
      --node[i - 1].size;
      write_node(node[i], pos[i]);

      // merge internal nodes
      merge_internal(node, i - 1, child, pos);
    }

  } else if (child[i] == node[i - 1].size) { // only left brother

    Node lt_bro;
    read_node(lt_bro, node[i - 1].child[node[i - 1].size - 1]);
    if (lt_bro.size > M / 2) { // borrow from left brother
      for (int j = node[i].size; j > 0; --j) {
        node[i].index[j] = node[i].index[j - 1];
      }
      for (int j = node[i].size + 1; j > 0; --j) {
        node[i].child[j] = node[i].child[j - 1];
      }
      node[i].size = M / 2;
      node[i].index[0] = node[i - 1].index[node[i - 1].size - 1];
      node[i].child[0] = lt_bro.child[lt_bro.size];
      node[i - 1].index[node[i - 1].size - 1] = lt_bro.index[lt_bro.size - 1];
      --lt_bro.size;

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(lt_bro, node[i - 1].child[node[i - 1].size - 1]);
      return;

    } else { // merge with left brother
      rec_node(pos[i]);

      lt_bro.index[lt_bro.size] = node[i - 1].index[node[i - 1].size - 1];
      for (int j = 0; j != node[i].size; ++j) {
        lt_bro.index[j + 1 + lt_bro.size] = node[i].index[j];
      }
      for (int j = 0; j <= node[i].size; ++j) {
        lt_bro.child[j + 1 + lt_bro.size] = node[i].child[j];
      }
      lt_bro.size += node[i].size + 1;
      --node[i - 1].size;
      write_node(lt_bro, node[i - 1].child[node[i - 1].size]);

      // merge internal nodes
      merge_internal(node, i - 1, child, pos);
    }

  } else { // 2 brothers

    Node lt_bro, rt_bro;
    read_node(rt_bro, node[i - 1].child[child[i] + 1]);
    if (rt_bro.size > M / 2) { // borrow from right brother

      node[i].index[node[i].size] = node[i - 1].index[child[i]];
      node[i - 1].index[child[i]] = rt_bro.index[0];
      ++node[i].size;
      node[i].child[node[i].size] = rt_bro.child[0];
      --rt_bro.size;
      for (int j = 0; j != rt_bro.size; ++j) {
        rt_bro.index[j] = rt_bro.index[j + 1];
      }
      for (int j = 0; j <= rt_bro.size; ++j) {
        rt_bro.child[j] = rt_bro.child[j + 1];
      }

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(rt_bro, node[i - 1].child[child[i] + 1]);
      return;
    }

    read_node(lt_bro, node[i - 1].child[child[i] - 1]);
    if (lt_bro.size > M / 2) { // borrow from left brother

      for (int j = node[i].size; j > 0; --j) {
        node[i].index[j] = node[i].index[j - 1];
      }
      for (int j = node[i].size + 1; j > 0; --j) {
        node[i].child[j] = node[i].child[j - 1];
      }
      ++node[i].size;
      node[i].index[0] = node[i - 1].index[child[i] - 1];
      node[i].child[0] = lt_bro.child[lt_bro.size];
      --lt_bro.size;
      node[i - 1].index[child[i] - 1] = lt_bro.index[lt_bro.size];

      write_node(node[i], pos[i]);
      write_node(node[i - 1], pos[i - 1]);
      write_node(lt_bro, node[i - 1].child[child[i] - 1]);
      return;
    }

    // merge with right brother

    rec_node(node[i - 1].child[child[i] + 1]);

    node[i].index[node[i].size] = node[i - 1].index[child[i]];
    for (int j = 0; j != rt_bro.size; ++j) {
      node[i].index[j + 1 + node[i].size] = rt_bro.index[j];
    }
    for (int j = 0; j <= rt_bro.size; ++j) {
      node[i].child[j + 1 + node[i].size] = rt_bro.child[j];
    }
    node[i].size += rt_bro.size + 1;

    for (int j = child[i]; j != node[i - 1].size - 1; ++j) {
      node[i - 1].index[j] = node[i - 1].index[j + 1];
    }
    for (int j = child[i] + 1; j != node[i - 1].size; ++j) {
      node[i - 1].child[j] = node[i - 1].child[j + 1];
    }
    --node[i - 1].size;
    write_node(node[i], pos[i]);

    // merge internal nodes
    merge_internal(node, i - 1, child, pos);
  }
}

template <typename Key> void BPlusTree<Key>::find(const Key &ind) {
  Node node;
  read_node(node, root);

  if (node.is_leaf && node.size == 0) {
    std::cout << "null\n";
    return;
  }

  while (!node.is_leaf) {
    if (ind > node.index[node.size - 1]) {
      read_node(node, node.child[node.size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int pos = lower_bound(node.index, node.size, ind);
      read_node(node, node.child[pos]);
    }
  }

  bool flag = true;

  while (true) {
    int res = 0;
    for (int i = 0; i != node.size; ++i) {
      res = strcmp(ind.str, node.index[i].str);
      if (res == 0) {
        flag = false;
        std::cout << node.index[i].val << ' ';
      } else if (res < 0) {
        break;
      }
    }
    if (res < 0) {
      break;
    }

    if (node.next == INT32_MAX) {
      break;
    }
    read_node(node, node.next);
  }

  if (flag) {
    std::cout << "null\n";
  } else {
    std::cout << '\n';
  }
}

template <typename Key> void BPlusTree<Key>::traverse() {
  Node node;
  u32 pos = root;
  int height = 1;
  ind_file.seekg(root);
  ind_file.read(reinterpret_cast<char *>(&node), sizeof(Node));

  while (!node.is_leaf) {
    if (node.size == 0) {
      std::cout << "SIZE ZERO!!!\n";
      return;
    }
    pos = node.child[0];
    ind_file.seekg(pos);
    ind_file.read(reinterpret_cast<char *>(&node), sizeof(Node));
    ++height;
  }

  std::cout << "HEIGHT: " << height << '\n';

  while (true) {
    std::cout << "ADDR: " << pos << " SIZE: " << node.size << '\n';
    for (int i = 0; i != node.size; ++i) {
      std::cout << node.index[i] << '\t';
    }
    std::cout << "\nEND OF BLOCK\n";
    if (node.next == INT32_MAX) {
      break;
    }
    pos = node.next;

    ind_file.seekg(pos);
    ind_file.read(reinterpret_cast<char *>(&node), sizeof(Node));
  }
}

template <typename Key>
int lower_bound(const Key *const beg, int size, const Key &index) {
  int start = 0;
  int last = size;
  while (start < last) {
    int mid = (start + last) / 2;
    if (beg[mid] >= index) {
      last = mid;
    } else {
      start = mid + 1;
    }
  }
  return start;
}

template <typename Key>
int upper_bound(const Key *const beg, int size, const Key &index) {
  int start = 0;
  int last = size;
  while (start < last) {
    int mid = (start + last) / 2;
    if (beg[mid] <= index) {
      start = mid + 1;
    } else {
      last = mid;
    }
  }
  return start;
}

#endif