#pragma once
#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "bpt.hpp"
#include "time.hpp"
#include <cstdint>

struct Train {
  // TODO: finish the structure
  char id[21] = {};
  int8_t stat_num = 0;
  char stations[100][31] = {};
  int seat_num = 0;
  int prices[99] = {};
  int start_t = 0;
  int16_t travel_t[99] = {};
  int16_t stop_t[98] = {};
  int start_sale = 0;
  int end_sale = 0;
  char type = 'A';

  Train &operator=(const Train &other) {
    if (&other == this) {
      return *this;
    }
    stat_num = other.stat_num;
    seat_num = other.seat_num;
    start_t = other.start_t;
    start_sale = other.start_sale;
    end_sale = other.end_sale;
    type = other.type;
    strcpy(id, other.id);
    for (int i = 0; i != stat_num; ++i) {
      strcpy(stations[i], other.stations[i]);
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

class TrSys {
  // TODO: add some menbers
public:
  bool add_train(const std::string &id, int stat_num, int seat_num,
                 const std::string *stations, const int *prices, int start_t,
                 const int *travel_t, const int *stop_t, int start_sale,
                 int end_sale, char type);

  bool delete_train(const std::string &id);

  bool release_train(const std::string &id);

  std::string query_train(const std::string &id, const Time &day);

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