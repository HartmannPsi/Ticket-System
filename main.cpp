#include "bpt.hpp"
#include <cstring>
#include <iostream>
#include <string>

int n;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::cin >> n;
  BPlusTree<Index> bpt;

  for (int i = 0; i != n; ++i) {
    std::string command;
    Index index;
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
      index.pos = INT32_MIN;
      bpt.find(index);
    } else if (command == "traverse") {
      bpt.traverse();
    }
  }

  return 0;
}