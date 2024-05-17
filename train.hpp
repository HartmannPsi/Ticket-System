#pragma once
#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "bpt.hpp"
#include "map.hpp"
#include "time.hpp"
#include <cstdint>
#include <fstream>

struct Train {
  // TODO: finish the structure
  char id[21] = {};
  int8_t stat_num = 0;
  int stations[100] = {};
  int seat_nums[99] = {};
  int prices[99] = {};
  int start_t = 0;
  int16_t travel_t[99] = {};
  int16_t stop_t[98] = {};
  int start_sale = 0;
  int end_sale = 0;
  char type = 'A';
  bool released = false;

  Train() {}

  Train &operator=(const Train &other) {
    if (&other == this) {
      return *this;
    }
    stat_num = other.stat_num;
    // seat_num = other.seat_num;
    start_t = other.start_t;
    start_sale = other.start_sale;
    end_sale = other.end_sale;
    type = other.type;
    strcpy(id, other.id);
    for (int i = 0; i != stat_num; ++i) {
      stations[i] = other.stations[i];
    }
    for (int i = 0; i != stat_num - 1; ++i) {
      prices[i] = other.prices[i];
      travel_t[i] = other.travel_t[i];
      seat_nums[i] = other.seat_nums[i];
    }
    for (int i = 0; i != stat_num - 2; ++i) {
      stop_t[i] = other.stop_t[i];
    }

    return *this;
  }

  bool operator==(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res == 0;
  }

  bool operator!=(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res != 0;
  }

  bool operator>(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res > 0;
  }

  bool operator<(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res < 0;
  }

  bool operator>=(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res >= 0;
  }

  bool operator<=(const Train &other) const {
    const int res = strcmp(id, other.id);
    return res <= 0;
  }
};

struct Station {
  int name = -1;
  char train_id[21] = {};

  Station &operator=(const Station &other) {
    if (&other == this) {
      return *this;
    }

    name = other.name;
    strcpy(train_id, other.train_id);

    return *this;
  }

  bool operator==(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    return name == other.name && res2 == 0;
  }

  bool operator!=(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    return name != other.name || res2 != 0;
  }

  bool operator>(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    if (name != other.name) {
      return name > other.name;
    } else {
      return res2 > 0;
    }
  }

  bool operator>=(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    if (name != other.name) {
      return name > other.name;
    } else {
      return res2 >= 0;
    }
  }

  bool operator<(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    if (name != other.name) {
      return name < other.name;
    } else {
      return res2 < 0;
    }
  }

  bool operator<=(const Station &other) const {
    const int res2 = strcmp(train_id, other.train_id);
    if (name != other.name) {
      return name < other.name;
    } else {
      return res2 <= 0;
    }
  }
};

struct Stat_serial {
  char name[31] = {};
  int serial = 0;
};

constexpr int M = 227; // M should be odd (101)

struct Node {
  Station index[M];
  u32 child[M];
  int size = 0;
  bool is_leaf = false;
  u32 next = 0;

  Node() {}
  ~Node() {}
};

constexpr u32 BSIZE = sizeof(Node);

class TrSys {
  BPlusTree<Train, 65> train_data;
  BPlusTree<Station, 227> stat_data;

  map<std::string, int> serials;
  map<int, std::string> stats;
  int max_serial = 0;
  std::fstream stat_file;

public:
  TrSys();

  ~TrSys();

  bool add_train(const std::string &id, int stat_num, int seat_num,
                 const std::string *stations, const int *prices, int start_t,
                 const int *travel_t, const int *stop_t, int start_sale,
                 int end_sale,
                 char type); // if stat_num == 2, then stop_t = nullptr

  bool delete_train(const std::string &id);

  bool release_train(const std::string &id);

  std::string query_train(const std::string &id, int day);

  std::string query_ticket(const std::string &from, const std::string &to,
                           int day,
                           bool tp); // tp: true for time, false for cost

  std::string query_transfer(const std::string &from, const std::string &to,
                             int day,
                             bool tp); // tp: true for time, false for cost

  int buy_ticket(
      const std::string &usr, const std::string &id, int day,
      const std::string &from, const std::string &to, int n,
      bool is_q); // -1 for failed, 0 for queue, positive for secceeded

  std::string query_order(const std::string &usr);

  bool refund_ticket(const std::string &usr, int n);

  void clear();
};

#endif