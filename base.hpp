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
  const std::string file_name;
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
      : index(index_file, index_recycle, r), file_name(data_file) {
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

  bool insert(const Val &val) {
    if (index.insert({val.id, end})) {
      write(val, end);
      end += sizeof(Val);
      return true;
    } else {
      return false;
    }
  }

  bool erase(const Val &val) { return index.erase({val.id, 0}); }

  bool modify(const Val &val) {
    Index ind(val.id, 0);

    if (!index.at(ind)) {
      return false;
    }

    write(val, ind.pos);
    return true;
  }

  bool at(Val &dest) {
    const u32 pos = index.find({dest.id, 0});

    if (pos == INT32_MAX) {
      return false;
    } else {
      read(dest, pos);
      return true;
    }
  }

  bool empty() { return index.empty(); }

  void clear() {
    index.clear();
    data.close();
    data.open(file_name, std::ios::out);
    data.close();
    data.open(file_name);
    end = 0;
  }
};

#endif