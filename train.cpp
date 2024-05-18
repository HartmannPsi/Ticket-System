#include "train.hpp"
#include "account.hpp"
#include <string>
// for test
#include <algorithm>

extern AcSys accounts;

TrSys::TrSys()
    : train_data("train.dat", "train.rec", "train.r", 4096 * 26, 512),
      stat_data("station.dat", "station.rec", "station.r"),
      every_train("every_train.dat", "every_train.rec", "every_train.r") {
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
  new_t.seat_num = seat_num;
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

  if (train_data.at(tmp)) {
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

  if (train_data.at(tmp)) {
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

      for (int day = tmp.start_sale; day <= tmp.end_sale; day += DAY) {
        EveryTr train;
        strcpy(train.id, tmp.id);
        train.day = day;
        for (int i = 0; i != tmp.stat_num - 1; ++i) {
          train.seat_nums[i] = tmp.seat_num;
        }
        every_train.insert(train);
      }

      return true;
    }
  } else {
    return false;
  }
}

std::string TrSys::query_train(const std::string &id,
                               int day) { // to be modified
  Train train;
  strcpy(train.id, id.c_str());
  if (train_data.at(train)) {

    if (train.start_sale <= day && day <= train.end_sale) {
      EveryTr tr;
      if (train.released) {
        strcpy(tr.id, train.id);
        tr.day = day;
        every_train.at(tr);
      }
      std::string res = "";
      res += std::string(train.id) + ' ' + train.type + '\n';

      Time time(train.start_t + day);

      const auto start_stat = stats[train.stations[0]];
      long long total_price = 0;
      res += start_stat + ' ';
      res += "xx-xx xx:xx -> ";
      res += time.display() + ' ';
      res += std::to_string(total_price) + ' ';
      if (!train.released) {
        res += std::to_string(train.seat_num) + '\n';
      } else {
        res += std::to_string(tr.seat_nums[0]) + '\n';
      }
      time += train.travel_t[0];
      total_price += train.prices[0];

      for (int i = 1; i != train.stat_num - 1; ++i) {
        const auto stat = stats[train.stations[i]];
        res += stat + ' ';
        res += time.display() + " -> ";
        time += train.stop_t[i - 1];
        res += time.display() + ' ';
        res += std::to_string(total_price) + ' ';
        if (!train.released) {
          res += std::to_string(train.seat_num) + '\n';
        } else {
          res += std::to_string(tr.seat_nums[i]) + '\n';
        }
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

template <>
vector<std::string> BPlusTree<Station, 227>::find_trains(const Station &ind) {
  Node node;
  read_node(node, root);
  vector<std::string> _res;

  if (node.is_leaf && node.size == 0) {
    // std::cout << "null\n";
    return _res;
  }

  while (!node.is_leaf) {
    if (ind > node.index[node.size - 1]) {
      read_node(node, node.child[node.size]);

    } else { // child[i]:  (index[i - 1], index[i]]
      int pos = lower_bound(node.index, node.size, ind);
      read_node(node, node.child[pos]);
    }
  }

  while (true) {
    // int res = 0;
    int i = 0;
    for (; i != node.size; ++i) {
      if (ind.name == node.index[i].name) {
        _res.push_back(std::string(node.index[i].train_id));
      } else if (ind.name < node.index[i].name) {
        break;
      }
    }
    if (ind.name < node.index[i].name) {
      break;
    }

    if (node.next == INT32_MAX) {
      break;
    }
    read_node(node, node.next);
  }

  return _res;
}

std::string TrSys::query_ticket(const std::string &from, const std::string &to,
                                int day,
                                bool tp) { // tp: true for time, false for cost
  const auto _from = serials[from], _to = serials[to];
  Station tmp;
  tmp.name = _from;
  auto train_ids = stat_data.find_trains(tmp);
  vector<Info> infos;

  for (int i = 0; i != train_ids.size(); ++i) {
    // auto &id = res[i];
    Train train;
    Info tmp;
    strcpy(train.id, train_ids[i].c_str());
    train_data.at(train);
    EveryTr tr;
    strcpy(tr.id, train_ids[i].c_str());

    // if (!(train.released && train.start_sale <= day && day <=
    // train.end_sale)) {
    //   continue;
    // }
    if (_from == train.stations[train.stat_num - 1]) {
      continue;
    }
    tmp.id = train_ids[i];
    Time time(day + train.start_t);
    int start_day = day;
    int j = 0;
    for (; j != train.stat_num - 1; ++j) {
      if (train.stations[j] == _from) {
        break;
      }
      time += train.travel_t[j];
      time += train.stop_t[j];
    }

    while (time.stamp() > day + DAY - 1) {
      start_day -= DAY;
      time -= DAY;
    }

    tmp.leave = time;
    tr.day = start_day;
    if (!every_train.at(tr)) {
      continue;
    }

    int seat = tr.seat_nums[j];
    int price = 0;
    for (; j != train.stat_num; ++j) {
      if (train.stations[j] == _to) {
        break;
      }
      if (j == train.stat_num - 1) {
        break;
      }
      time += train.travel_t[j];
      time += train.stop_t[j];
      seat = std::min(seat, tr.seat_nums[j]);
      price += train.prices[j];
    }
    if (train.stations[j] != _to) {
      continue;
    }
    tmp.arrive = time;
    tmp.seat = seat;
    tmp.price = price;
    infos.push_back(tmp);

    // get infos
  }
  // sort by key
  if (tp) {
    std::sort(infos.begin(), infos.end(), Info::CmpByTime());
  } else {
    std::sort(infos.begin(), infos.end(), Info::CmpByCost());
  }

  std::string res = "";
  res += std::to_string(infos.size()) + '\n';
  for (int i = 0; i != infos.size(); ++i) {
    res += infos[i].id + ' ';
    res += from + ' ';
    res += infos[i].leave.display() + " -> ";
    res += to + ' ';
    res += infos[i].arrive.display() + ' ';
    res += std::to_string(infos[i].price) + ' ';
    res += std::to_string(infos[i].seat) + '\n';
  }

  return res;
  // return res
}

std::string
TrSys::query_transfer(const std::string &from, const std::string &to, int day,
                      bool tp) { // tp: true for time, false for cost
}

int TrSys::buy_ticket(
    const std::string &usr, const std::string &id, int day,
    const std::string &from, const std::string &to, int n,
    bool is_q) { // -1 for failed, 0 for queue, positive for secceeded

  if (accounts.usrpriv(usr) == -1) {
    return -1;
  }

  Train train;
  strcpy(train.id, id.c_str());

  if (train_data.at(train) && train.released) {
    const auto _from = serials[from], _to = serials[to];
    EveryTr tr;
    strcpy(tr.id, id.c_str());

    Time time(day + train.start_t);
    int start_day = day;
    int j = 0;
    for (; j != train.stat_num - 1; ++j) {
      if (train.stations[j] == _from) {
        break;
      }
      time += train.travel_t[j];
      time += train.stop_t[j];
    }

    while (time.stamp() > day + DAY - 1) {
      start_day -= DAY;
      time -= DAY;
    }

    tr.day = start_day;
    if (!every_train.at(tr)) {
      return -1;
    }

    int price = 0;
    for (; j != train.stat_num; ++j) {
      if (train.stations[j] == _to) {
        break;
      }
      if (j == train.stat_num - 1) {
        break;
      }
      time += train.travel_t[j];
      time += train.stop_t[j];
      if (tr.seat_nums[j] < n) {
        if (is_q) {
          // add to queue
          // add to user order
          return 0;
        }
      }
      price += train.prices[j] * n;
      tr.seat_nums[j] -= n;
    }

    every_train.modify(tr);

    // add to user order
    return price;

  } else {
    return -1;
  }
}

std::string TrSys::query_order(const std::string &usr) {}

bool TrSys::refund_ticket(const std::string &usr, int n) {}

void TrSys::clear() {
  train_data.clear();
  stat_data.clear();
  every_train.clear();
  serials.clear();
  stats.clear();
  max_serial = 0;
}