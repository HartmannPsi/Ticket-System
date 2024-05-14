#pragma once
#ifndef BPT_HPP
#define BPT_HPP

#include "double_list.hpp"
#include "map.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using u32 = unsigned long;

template <typename Key, const int M> class BPlusTree {

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
  // const int M; // M should be odd (101)
  const u32 BLOCK;
  const int CACHE_SIZE;

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
    List<pair<Node, u32>> list;
    map<u32, typename List<pair<Node, u32>>::_Node *> table;
    BPlusTree<Key, M> *pt;

  public:
    LRUCache(BPlusTree<Key, M> *_pt) : pt(_pt) {}

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

      if (list.size() > pt->CACHE_SIZE) {
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
            const std::string &r = ".root", u32 _BLOCK = 4096 * 2,
            int _CACHE = 2048);

  ~BPlusTree();

  bool insert(const Key &ind);

  bool erase(const Key &ind);

  u32 find(const Key &ind);

  void traverse();

  int count(const Key &ind);

  bool at(Key &ind);

  bool modify(const Key &ind);

  bool empty() {
    Node node;
    read_node(node, root);
    return node.size == 0;
  }
};

template <typename Key>
int lower_bound(const Key *const beg, int size, const Key &index);

template <typename Key>
int upper_bound(const Key *const beg, int size, const Key &index);

template <typename Key, const int M>
BPlusTree<Key, M>::BPlusTree(const std::string &index_file,

                             const std::string &index_recycle,

                             const std::string &r, u32 _BLOCK, int _CACHE)
    : cache(this), ind(index_file), BLOCK(_BLOCK), CACHE_SIZE(_CACHE) {

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

template <typename Key, const int M> BPlusTree<Key, M>::~BPlusTree() {

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
    // i += sizeof(u32);
  }

  ind_file.close();
  ind_rec.close();
  _root.close();
}

template <typename Key, const int M>
void BPlusTree<Key, M>::read_node(Node &dest, u32 pos) {
  auto read = cache.find(pos);
  dest = *read;
}

template <typename Key, const int M>
void BPlusTree<Key, M>::write_node(const Node &src, u32 pos) {
  cache.modify(pos, src);
}

template <typename Key, const int M> u32 BPlusTree<Key, M>::free_node() {
  u32 res = ind_pool.top();
  ind_pool.pop_front();
  if (ind_pool.empty()) {
    ind_pool.push_front(res + BLOCK);
  }

  return res;
}

template <typename Key, const int M> void BPlusTree<Key, M>::rec_node(u32 pos) {
  cache.erase(pos);
  ind_pool.push_front(pos);
}

template <typename Key, const int M>
bool BPlusTree<Key, M>::insert(const Key &ind) {
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

  int tmp = lower_bound(node[i].index, node[i].size, ind);
  if (node[i].size > 0 && tmp < node[i].size && node[i].index[tmp] == ind) {
    return false;
  }

  if (node[i].size == 0) { // root
    node[i].index[0] = ind;
    ++node[i].size;
    write_node(node[i], pos[i]);
    return true;
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
    return true;
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

  return true;
}

template <typename Key, const int M>
void BPlusTree<Key, M>::insert_internal(Node node[], int i, int child[],
                                        u32 pos[], u32 new_pos,
                                        const Key &new_ind) {
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

template <typename Key, const int M>
int BPlusTree<Key, M>::count(const Key &ind) {
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

template <typename Key, const int M>
bool BPlusTree<Key, M>::erase(const Key &ind) {
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
    return false;
  }

  if (ind > node[i].index[node[i].size - 1]) {
    return false;
  }
  int _pos = lower_bound(node[i].index, node[i].size, ind);

  if (node[i].index[_pos] != ind) {
    return false;
  }

  for (int j = _pos; j != node[i].size - 1; ++j) {
    node[i].index[j] = node[i].index[j + 1];
  }
  --node[i].size;

  if (node[i].size >= M / 2 || i == 0) { // node == root OR node size > 50
    write_node(node[i], pos[i]);
    return true;
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
      return true;

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
      return true;

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
      return true;
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
      return true;
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

  return true;
}

template <typename Key, const int M>
void BPlusTree<Key, M>::merge_internal(Node node[], int i, int child[],
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

template <typename Key, const int M>
u32 BPlusTree<Key, M>::find(const Key &ind) {
  Node node;
  read_node(node, root);

  if (node.is_leaf && node.size == 0) {
    // std::cout << "null\n";
    return INT32_MAX;
  }

  while (!node.is_leaf) {
    if (ind > node.index[node.size - 1]) {
      read_node(node, node.child[node.size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int pos = lower_bound(node.index, node.size, ind);
      read_node(node, node.child[pos]);
    }
  }

  while (true) {
    int res = 0;
    for (int i = 0; i != node.size; ++i) {
      res = strcmp(ind.str, node.index[i].str);
      if (res == 0) {

        return node.index[i].val;
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

  return INT32_MAX;
}

template <typename Key, const int M> bool BPlusTree<Key, M>::at(Key &ind) {
  Node node;
  read_node(node, root);

  if (node.is_leaf && node.size == 0) {
    // std::cout << "null\n";
    return false;
  }

  while (!node.is_leaf) {
    if (ind > node.index[node.size - 1]) {
      read_node(node, node.child[node.size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int pos = lower_bound(node.index, node.size, ind);
      read_node(node, node.child[pos]);
    }
  }

  for (int i = 0; i != node.size; ++i) {
    if (node.index[i] == ind) {
      ind = node.index[i];
      return true;
    }
  }

  return false;
}

template <typename Key, const int M>
bool BPlusTree<Key, M>::modify(const Key &ind) {
  Node node;
  read_node(node, root);
  u32 pos = root;

  if (node.is_leaf && node.size == 0) {
    // std::cout << "null\n";
    return false;
  }

  while (!node.is_leaf) {
    if (ind > node.index[node.size - 1]) {
      read_node(node, node.child[node.size]);
      pos = node.child[node.size];

    } else { // child[i]:  (index[i - 1], index[i]]
      int _pos = lower_bound(node.index, node.size, ind);
      read_node(node, node.child[_pos]);
      pos = node.child[_pos];
    }
  }

  for (int i = 0; i != node.size; ++i) {
    if (node.index[i] == ind) {
      node.index[i] = ind;
      write_node(node, pos);
      return true;
    }
  }

  return false;
}

template <typename Key, const int M> void BPlusTree<Key, M>::traverse() {
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

struct Index {
  char str[22] = {};
  u32 pos = 0;

  Index(const std::string &s, int n) : pos(n) { strcpy(str, s.c_str()); }

  Index() {}

  ~Index() {}

  Index &operator=(const Index &other) {
    if (&other == this) {
      return *this;
    }
    strcpy(str, other.str);
    pos = other.pos;

    return *this;
  }

  bool operator==(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    return res == 0;
  }

  bool operator!=(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    return res != 0 || pos != other.pos;
  }

  bool operator>(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res > 0;
    else
      return pos > other.pos;
  }

  bool operator<(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res < 0;
    else
      return pos < other.pos;
  }

  bool operator>=(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res > 0;
    else
      return pos >= other.pos;
  }

  bool operator<=(const Index &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res < 0;
    else
      return pos <= other.pos;
  }

  friend std::istream &operator>>(std::istream &is, Index &obj) {
    std::string s;
    is >> s >> obj.pos;
    strcpy(obj.str, s.c_str());
    return is;
  }

  friend std::ostream &operator<<(std::ostream &os, const Index &obj) {
    os << obj.str << ' ' << obj.pos;
    return os;
  }
};

#endif