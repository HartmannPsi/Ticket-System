#pragma once
#ifndef MAP_HPP
#define MAP_HPP

#include "utility.hpp"
#include <string>

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

#endif