#pragma once
#ifndef DOUBLE_LIST_HPP
#define DOUBLE_LIST_HPP

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

#endif