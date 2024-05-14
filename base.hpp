#pragma once
#ifndef BASE_HPP
#define BASE_HPP

#include "bpt.hpp"
#include <cstring>
#include <fstream>
#include <string>

template <typename Val, const int M = 203> class Base {
  BPlusTree<Index, M> index;
  std::fstream data;
  u32 end;

  void read(Val &dest, u32 pos) {
    data.seekg(pos);
    data.read(reinterpret_cast<char *>(&dest), sizeof(Val));
  }

  void write(const Val &src, u32 pos) {
    data.seekp(pos);
    data.write(reinterpret_cast<const char *>(&src), sizeof(Val));
  }

public:
  Base(const std::string &index_file, const std::string &index_recycle,
       const std::string &r, const std::string &data_file)
      : index(index_file, index_recycle, r) {
    data.open(data_file);

    if (!data.is_open()) {
      data.open((data_file), std::ios::out);
      data.close();
      data.open(data_file);
      end = sizeof(u32);
    } else {
      data.seekg(0);
      data.read(reinterpret_cast<char *>(&end), sizeof(u32));
    }
  }

  ~Base() {
    data.seekp(0);
    data.write(reinterpret_cast<const char *>(&end), sizeof(u32));
    data.close();
  }

  bool insert(const std::string &str, const Val &val) {
    if (index.insert({str.c_str(), end})) {
      write(val, end);
      end += sizeof(Val);
      return true;
    } else {
      return false;
    }
  }

  bool erase(const std::string &str) { return index.erase({str.c_str(), 0}); }

  bool modify(const std::string &str, const Val &val) {
    const u32 pos = index.find({str.c_str(), 0});

    if (pos == INT32_MAX) {
      return false;
    } else {
      write(val, pos);
      return true;
    }
  }

  bool find(const std::string &str, Val &dest) {
    const u32 pos = index.find({str.c_str(), 0});

    if (pos == INT32_MAX) {
      return false;
    } else {
      read(dest, pos);
      return true;
    }
  }
};

#endif