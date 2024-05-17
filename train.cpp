#include "train.hpp"
#include <string>

TrSys::TrSys()
    : train_data("train.dat", "train.rec", "train.r", 4096 * 20, 512),
      stat_data("station.dat", "station.rec", "station.r") {
  stat_file.open("station_serials.dat");

  if (!stat_file.is_open()) {
    stat_file.open("station_serials.dat", std::ios::out);
    max_serial = 0;

  } else {
    stat_file.seekg(0);
    stat_file.read(reinterpret_cast<char *>(&max_serial), sizeof(int));
    stat_file.seekg(0, std::ios::end);
    const u32 end = stat_file.tellg();

    for (u32 i = sizeof(int); i < end; i += sizeof(Stat_serial)) {
      Stat_serial stat;
      stat_file.seekg(i);
      stat_file.read(reinterpret_cast<char *>(&stat), sizeof(Stat_serial));
      serials.insert({std::string(stat.name), stat.serial});
      stats.insert({stat.serial, std::string(stat.name)});
    }

    stat_file.open("station_serials.dat", std::ios::out);
  }
}

TrSys::~TrSys() {
  stat_file.seekp(0);
  stat_file.write(reinterpret_cast<const char *>(&max_serial), sizeof(int));

  u32 i = sizeof(int);
  for (auto it = stats.cbegin(); it != stats.cend();
       ++it, i += sizeof(Stat_serial)) {
    Stat_serial stat;
    stat.serial = it->first;
    strcpy(stat.name, it->second.c_str());
    stat_file.seekp(i);
    stat_file.write(reinterpret_cast<const char *>(&stat), sizeof(Stat_serial));
  }

  stat_file.close();
}

bool TrSys::add_train(const std::string &id, int stat_num, int seat_num,
                      const std::string *stations, const int *prices,
                      int start_t, const int *travel_t, const int *stop_t,
                      int start_sale, int end_sale,
                      char type) { // if stat_num == 2, then stop_t = nullptr

  Train new_t;
  new_t.released = false;
  new_t.stat_num = stat_num;
  // new_t.seat_num = seat_num;
  new_t.start_t = start_t;
  new_t.start_sale = start_sale;
  new_t.end_sale = end_sale;
  new_t.type = type;
  strcpy(new_t.id, id.c_str());
  for (int i = 0; i != stat_num; ++i) {
    stats.insert({max_serial, stations[i]});
    serials.insert({stations[i], max_serial});
    new_t.stations[i] = max_serial;
    ++max_serial;
    // strcpy(new_t.stations[i], stations[i].c_str());
  }

  for (int i = 0; i != stat_num - 1; ++i) {
    new_t.prices[i] = prices[i];
    new_t.travel_t[i] = travel_t[i];
    new_t.seat_nums[i] = seat_num;
  }

  if (stat_num != 2) {
    for (int i = 0; i != stat_num - 2; ++i) {
      new_t.stop_t[i] = stop_t[i];
    }
  }

  if (train_data.insert(new_t)) {
    return true;
  } else {
    return false;
  }
}

bool TrSys::delete_train(const std::string &id) {
  Train tmp;
  strcpy(tmp.id, id.c_str());

  if (train_data.find(tmp)) {
    if (tmp.released) {
      return false;
    } else {
      train_data.erase(tmp);
      return true;
    }
  } else {
    return false;
  }
}

bool TrSys::release_train(const std::string &id) {
  Train tmp;
  strcpy(tmp.id, id.c_str());

  if (train_data.find(tmp)) {
    if (tmp.released) {
      return false;
    } else {
      tmp.released = true;
      train_data.modify(tmp);
      for (int i = 0; i != tmp.stat_num; ++i) {
        Station stat;
        stat.name = tmp.stations[i];
        strcpy(stat.train_id, tmp.id);
        stat_data.insert(stat);
      }
      return true;
    }
  } else {
    return false;
  }
}

std::string TrSys::query_train(const std::string &id, int day) {
  Train train;
  strcpy(train.id, id.c_str());
  if (train_data.find(train)) {

    if (train.start_sale <= day && day <= train.end_sale) {
      std::string res = "";
      res += std::string(train.id) + ' ' + train.type + '\n';

      Time time(train.start_t + day);

      const auto start_stat = stats[train.stations[0]];
      long long total_price = 0;
      res += start_stat + ' ';
      res += "xx-xx xx:xx -> ";
      res += time.display() + ' ';
      res += std::to_string(total_price) + ' ';
      res += std::to_string(train.seat_nums[0]) + '\n';
      time += train.travel_t[0];
      total_price += train.prices[0];

      for (int i = 1; i != train.stat_num - 1; ++i) {
        const auto stat = stats[train.stations[i]];
        res += stat + ' ';
        res += time.display() + " -> ";
        time += train.stop_t[i - 1];
        res += time.display() + ' ';
        res += std::to_string(total_price) + ' ';
        res += std::to_string(train.seat_nums[i]) + '\n';
        time += train.travel_t[i];
        total_price += train.prices[i];
      }

      const auto end_stat = stats[train.stations[train.stat_num - 1]];
      res += end_stat + ' ';
      res += time.display() + " -> ";
      res += "xx-xx xx:xx ";
      res += std::to_string(total_price) + ' ';
      res += "x\n";

      return res;
    } else {
      return "";
    }

  } else {
    return "";
  }
}

std::string TrSys::query_ticket(const std::string &from, const std::string &to,
                                int day,
                                bool tp) { // tp: true for time, false for cost
}

std::string
TrSys::query_transfer(const std::string &from, const std::string &to, int day,
                      bool tp) { // tp: true for time, false for cost
}

int TrSys::buy_ticket(
    const std::string &usr, const std::string &id, int day,
    const std::string &from, const std::string &to, int n,
    bool is_q) { // -1 for failed, 0 for queue, positive for secceeded
}

std::string TrSys::query_order(const std::string &usr) {}

bool TrSys::refund_ticket(const std::string &usr, int n) {}

void TrSys::clear() {
  train_data.clear();
  stat_data.clear();
  serials.clear();
  stats.clear();
  max_serial = 0;
}