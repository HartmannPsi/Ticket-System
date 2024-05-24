#pragma once
#ifndef BASE_HPP
#define BASE_HPP

#include "bpt.hpp"
#include <cstring>
#include <fstream>
#include <string>

extern int TIME;

template <typename Val, const int M = 101> class Base {
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
  Base(const std::string &index_file, const std::string &r,
       const std::string &data_file)
      : index(index_file, r, 800), file_name(data_file) {
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
    Index ind({val.id, end});

    if (index.at(ind)) {
      return false;
    }

    if (index.insert(ind)) {
      write(val, end);
      end += sizeof(Val);
      return true;
    } else {
      return false;
    }
  }

  bool erase(const Val &val) {
    Index ind(val.id, 0);

    // if (TIME == 199054) {
    //   std::cout << ind.str << ' ' << ind.pos << '\n';
    // }

    return index.erase(ind);
  }

  bool modify(const Val &val) {
    Index ind(val.id, 0);

    if (!index.at(ind)) {
      return false;
    }

    write(val, ind.pos);
    return true;
  }

  bool at(Val &dest) {

    Index ind(dest.id, 0);
    // const u32 pos = index.find({dest.id, 0});

    if (!index.at(ind)) {
      return false;
    } else {
      read(dest, ind.pos);
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