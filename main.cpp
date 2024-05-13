#include "bpt.hpp"
#include <cstring>
#include <iostream>
#include <string>

struct Data {
  char str[65] = {};
  int val = 0;

  Data(const std::string &s, int n) : val(n) { strcpy(str, s.c_str()); }

  Data() {}

  ~Data() {}

  Data &operator=(const Data &other) {
    if (&other == this) {
      return *this;
    }
    strcpy(str, other.str);
    val = other.val;

    return *this;
  }

  bool operator==(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    return res == 0 && val == other.val;
  }

  bool operator!=(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    return res != 0 || val != other.val;
  }

  bool operator>(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res > 0;
    else
      return val > other.val;
  }

  bool operator<(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res < 0;
    else
      return val < other.val;
  }

  bool operator>=(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res > 0;
    else
      return val >= other.val;
  }

  bool operator<=(const Data &other) const {
    const auto &res = strcmp(str, other.str);
    if (res != 0)
      return res < 0;
    else
      return val <= other.val;
  }

  friend std::istream &operator>>(std::istream &is, Data &obj) {
    std::string s;
    is >> s >> obj.val;
    strcpy(obj.str, s.c_str());
    return is;
  }

  friend std::ostream &operator<<(std::ostream &os, const Data &obj) {
    os << obj.str << ' ' << obj.val;
    return os;
  }
};

struct Node {
  Data index[M];
  u32 child[M];
  int size = 0;
  bool is_leaf = false;
  u32 next = 0;

  Node() {}
  ~Node() {}
};

int n;
constexpr u32 BSIZE = sizeof(Node);

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::cin >> n;
  BPlusTree<Data> bpt;

  for (int i = 0; i != n; ++i) {
    std::string command;
    Data index;
    std::cin >> command;

    if (command == "insert") {
      std::cin >> index;
      bpt.insert(index);
    } else if (command == "delete") {
      std::cin >> index;
      bpt.erase(index);

    } else if (command == "find") {
      std::string tmp;
      std::cin >> tmp;
      strcpy(index.str, tmp.c_str());
      index.val = INT32_MIN;
      bpt.find(index);
    } else if (command == "traverse") {
      bpt.traverse();
    }
  }

  return 0;
}