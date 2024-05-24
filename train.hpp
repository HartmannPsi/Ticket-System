#pragma once
#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "account.hpp"
#include "base.hpp"
#include "bpt.hpp"
#include "map.hpp"
#include "time.hpp"
#include "vector.hpp"
#include <cstdint>
#include <fstream>

struct Train {
  // TODO: finish the structure
  char id[21] = {};
  int8_t stat_num = 0;
  int stations[100] = {};
  int seat_num = 0;
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
    seat_num = other.seat_num;
    strcpy(id, other.id);
    for (int i = 0; i != stat_num; ++i) {
      stations[i] = other.stations[i];
    }
    for (int i = 0; i != stat_num - 1; ++i) {
      prices[i] = other.prices[i];
      travel_t[i] = other.travel_t[i];
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

struct EveryTr {
  char id[21] = {};
  int seat_nums[99] = {};
  int day = 0;

  bool operator==(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    return res == 0 && day == other.day;
  }

  bool operator!=(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    return res != 0 || day != other.day;
  }

  bool operator<(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res < 0;
    } else {
      return day < other.day;
    }
  }

  bool operator<=(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res < 0;
    } else {
      return day <= other.day;
    }
  }

  bool operator>(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res > 0;
    } else {
      return day > other.day;
    }
  }

  bool operator>=(const EveryTr &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res > 0;
    } else {
      return day >= other.day;
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const EveryTr &obj) {
    os << obj.id << ' ' << Time(obj.day).display() << '\n';
    return os;
  }
};

struct EveryTrIndex {
  char id[21] = {};
  // int seat_nums[99] = {};
  int day = 0;
  u32 pos = 0;

  EveryTrIndex() {}

  EveryTrIndex(const EveryTr &other) {
    strcpy(id, other.id);
    day = other.day;
  }

  bool operator==(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    return res == 0 && day == other.day;
  }

  bool operator!=(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    return res != 0 || day != other.day;
  }

  bool operator<(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res < 0;
    } else {
      return day < other.day;
    }
  }

  bool operator<=(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res < 0;
    } else {
      return day <= other.day;
    }
  }

  bool operator>(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res > 0;
    } else {
      return day > other.day;
    }
  }

  bool operator>=(const EveryTrIndex &other) const {
    const int res = strcmp(id, other.id);
    if (res != 0) {
      return res > 0;
    } else {
      return day >= other.day;
    }
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

struct Info {
  std::string id;
  int price, seat;
  Time leave, arrive;
  struct CmpByTime {
    bool operator()(const Info &lhs, const Info &rhs) const {
      const auto t1 = lhs.arrive.stamp() - lhs.leave.stamp(),
                 t2 = rhs.arrive.stamp() - rhs.leave.stamp();
      if (t1 != t2) {
        return t1 < t2;
      } else {
        return lhs.id < rhs.id;
      }
    }
  };
  struct CmpByCost {
    bool operator()(const Info &lhs, const Info &rhs) const {
      if (lhs.price != rhs.price) {
        return lhs.price < rhs.price;
      } else {
        return lhs.id < rhs.id;
      }
    }
  };
  Info() : leave(0), arrive(0) {}
  ~Info() {}
};

struct Stat_serial {
  char name[31] = {};
  int serial = 0;

  Stat_serial() {}

  Stat_serial(const std::string &str) { strcpy(name, str.c_str()); }

  bool operator==(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res == 0;
  }

  bool operator!=(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res != 0;
  }

  bool operator<=(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res <= 0;
  }

  bool operator>=(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res >= 0;
  }

  bool operator<(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res < 0;
  }

  bool operator>(const Stat_serial &rhs) const {
    const int res = strcmp(name, rhs.name);
    return res > 0;
  }
};

struct StatMemoryRiver {
  std::fstream file;

  StatMemoryRiver() {
    file.open("stats");
    if (!file.is_open()) {
      file.open("stats", std::ios::out);
      file.close();
      file.open("stats");
    }
  }

  ~StatMemoryRiver() { file.close(); }

  bool at(Stat_serial &dest) {
    u32 pos = u32(dest.serial) * sizeof(Stat_serial);
    file.seekg(pos);
    file.read(reinterpret_cast<char *>(&dest), sizeof(Stat_serial));
    return true;
  }

  bool insert(const Stat_serial &ind) {
    u32 pos = u32(ind.serial) * sizeof(Stat_serial);
    file.seekp(pos);
    file.write(reinterpret_cast<const char *>(&ind), sizeof(Stat_serial));
    return true;
  }

  void clear() {
    file.close();
    file.open("stats", std::ios::out);
    file.close();
    file.open("stats");
  }

  void traverse(int max_serial) {
    u32 pos = 0;
    Stat_serial tmp;
    for (; pos != u32(max_serial) * sizeof(Stat_serial);
         pos += sizeof(Stat_serial)) {
      file.seekg(pos);
      file.read(reinterpret_cast<char *>(&tmp), sizeof(Stat_serial));
      std::cout << tmp.serial << ' ' << tmp.name << '\n';
    }
  }

  std::string operator[](int n) {
    Stat_serial tmp;
    tmp.serial = n;
    at(tmp);
    return std::string(tmp.name);
  }
};

struct Queue;

struct History {
  char usr[21] = {};
  char tr_id[21] = {};
  int time = 0, num = 0, from = 0, to = 0, start_day = 0, price = 0, leave = 0,
      arrive = 0;
  int8_t status = 0; // 0 for pending, 1 for success, -1 for refunded

  History(const Queue &other);

  History() {}

  friend std::ostream &operator<<(std::ostream &os, const History &obj) {
    os << obj.usr << ' ' << obj.time;
    return os;
  }

  bool operator==(const History &other) const {
    const int res = strcmp(usr, other.usr);

    return res == 0 && time == other.time;
  }

  bool operator!=(const History &other) const {
    const int res = strcmp(usr, other.usr);

    return res != 0 || time != other.time;
  }

  bool operator<=(const History &other) const {
    const int res = strcmp(usr, other.usr);
    if (res != 0) {
      return res < 0;
    } else {
      return time >= other.time;
    }
  }

  bool operator>=(const History &other) const {
    const int res = strcmp(usr, other.usr);
    if (res != 0) {
      return res > 0;
    } else {
      return time <= other.time;
    }
  }

  bool operator<(const History &other) const {
    const int res = strcmp(usr, other.usr);
    if (res != 0) {
      return res < 0;
    } else {
      return time > other.time;
    }
  }

  bool operator>(const History &other) const {
    const int res = strcmp(usr, other.usr);
    if (res != 0) {
      return res > 0;
    } else {
      return time < other.time;
    }
  }
};

struct Queue {
  char usr[21] = {};
  char tr_id[21] = {};
  int time = 0, num = 0, from = 0, to = 0,
      start_day = 0; // price = 0, leave = 0,
                     // arrive = 0;
  // int8_t status = 0; // 0 for pending, 1 for success, -1 for refunded

  Queue(const History &other) {
    strcpy(usr, other.usr);
    strcpy(tr_id, other.tr_id);
    time = other.time;
    num = other.num;
    from = other.from;
    to = other.to;
    start_day = other.start_day;
  }

  Queue() {}

  friend std::ostream &operator<<(std::ostream &os, const Queue &obj) {
    os << obj.time << ' ' << obj.usr << ' ' << obj.tr_id;
    return os;
  }

  bool operator==(const History &other) const { return time == other.time; }
  bool operator!=(const History &other) const { return time != other.time; }
  bool operator>(const History &other) const { return time > other.time; }
  bool operator<(const History &other) const { return time < other.time; }
  bool operator>=(const History &other) const { return time >= other.time; }
  bool operator<=(const History &other) const { return time <= other.time; }
};

template <> class Base<EveryTr> { // M = 169
  BPlusTree<EveryTrIndex, 85> index;
  std::fstream data;
  const std::string file_name;
  u32 end;
  // using Val = EveryTr;

  void read(EveryTr &dest, u32 pos) {
    data.seekg(pos);
    data.read(reinterpret_cast<char *>(&dest), sizeof(EveryTr));
  }

  void write(const EveryTr &src, u32 pos) {
    data.seekp(pos);
    data.write(reinterpret_cast<const char *>(&src), sizeof(EveryTr));
  }

public:
  Base(const std::string &index_file, const std::string &r,
       const std::string &data_file)
      : index(index_file, r), file_name(data_file) {
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

  bool insert(const EveryTr &val) {
    EveryTrIndex ind(val);
    ind.pos = end;

    if (index.at(ind)) {
      return false;
    }

    if (index.insert(ind)) {
      write(val, end);
      end += sizeof(EveryTr);
      return true;
    } else {
      return false;
    }
  }

  bool erase(const EveryTr &val) {
    EveryTrIndex ind(val);
    return index.erase(ind);
  }

  bool modify(const EveryTr &val) {
    EveryTrIndex ind(val);

    if (!index.at(ind)) {
      return false;
    }

    write(val, ind.pos);
    return true;
  }

  bool at(EveryTr &dest) {

    EveryTrIndex ind(dest);
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

constexpr int M = 35; // M should be odd (101)

struct Node {
  Account index[M];
  u32 child[M];
  int size = 0;
  bool is_leaf = false;
  u32 next = 0;

  Node() {}
  ~Node() {}
};

constexpr u32 BSIZE = sizeof(Node);

class TrSys {
  // BPlusTree<Train, 79> train_data; // 4096 * 24
  Base<Train> train_data;
  BPlusTree<Station, 113> stat_data; // TODO: adjust the numbers
  Base<EveryTr> every_train;
  BPlusTree<History, 45> history;
  BPlusTree<Queue, 55> queue;

  BPlusTree<Stat_serial, 91> serials;
  StatMemoryRiver stats;
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
      const std::string &from, const std::string &to, int n, bool is_q,
      int time_stamp); // -1 for failed, 0 for queue, positive for secceeded

  std::string query_order(const std::string &usr);

  bool refund_ticket(const std::string &usr, int n);

  void clear();

  bool queue_ticket(const Queue &q);

  vector<pair<int, int>>
  is_intersect(const Train &t1, const Train &t2, int from_serial,
               int to_serial); // -1 if not, a rank in **t1, t2** otherwise

  int get_rank(const Train &t, int stat_serial);

  int total_time(const Train &t, int stat_serial);

  int total_cost(const Train &t, int stat_serial);

  int max_seat(const EveryTr &t, int from_serial, int to_serial);

  // void traverse_everytr() { every_train.traverse(); }
  /*
    void traverse_stations() {
      for (auto it = stats.cbegin(); it != stats.cend(); ++it) {
        // if (it->second == "江西省瑞昌市" || it->second == "海南省三亚市") {
        //   std::cout << "\nLOOK!\n";
        // }
        std::cout << it->first << ' ' << it->second << '\n';
      }

      std::cout << "===================\n";

      for (auto it = serials.cbegin(); it != serials.cend(); ++it) {
        // if (it->first == "江西省瑞昌市" || it->first == "海南省三亚市") {
        //   std::cout << "\nLOOK!\n";
        // }
        std::cout << it->first << ' ' << it->second << '\n';
      }

      std::cout << "max_serial: " << max_serial << '\n';
    }*/
};

#endif