#include "train.hpp"
#include "account.hpp"
#include "time.hpp"
#include <iterator>
#include <string>
// for test
#include <algorithm>

extern AcSys accounts;

TrSys::TrSys()
    : train_data("train.dat", "train.rec", "train.r", 4096 * 26, 512),
      stat_data("station.dat", "station.rec", "station.r"),
      every_train("every_train.dat", "every_train.rec", "every_train.r"),
      history("history.dat", "history.rec", "history.r"),
      queue("queue.dat", "queue.rec", "queue.r") {

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

pair<int, int> TrSys::is_intersect(
    const Train &t1, const Train &t2, int from_serial,
    int to_serial) { // -1 if not, a rank in **t1, t2** otherwise
  map<int, int> stats;
  for (int i = from_serial + 1; i != t1.stat_num; ++i) {
    const int &stat_serial = t1.stations[i];
    stats.insert(
        {stat_serial,
         i}); // first: serial number of the station; second: the rank in t1
  }

  for (int i = 0; i != to_serial; ++i) {
    const int &stat_serial = t2.stations[i];
    const auto it = stats.find(stat_serial);
    if (it != stats.end()) {
      return {it->second, i};
    }
  }

  return {-1, -1};
}

int TrSys::get_rank(const Train &t, int stat_serial) {
  for (int i = 0; i != t.stat_num; ++i) {
    if (t.stations[i] == stat_serial) {
      return i;
    }
  }
  return -1;
}

int TrSys::total_time(
    const Train &t,
    int stat_serial) { // time from start station to station required
  if (stat_serial == 0) {
    return 0;
  }
  int res = t.travel_t[0];
  for (int i = 1; i != stat_serial; ++i) {
    res += t.travel_t[i];
    res += t.stop_t[i - 1];
  }

  return res;
}

int TrSys::total_cost(
    const Train &t,
    int stat_serial) { // cost from start station to station required
  int res = 0;
  for (int i = 0; i != stat_serial; ++i) {
    res += t.prices[i];
  }
  return res;
}

int TrSys::max_seat(const EveryTr &t, int from_serial, int to_serial) {
  int res = INT32_MAX;
  for (int i = from_serial; i != to_serial; ++i) {
    res = std::min(res, t.seat_nums[i]);
  }
  return res;
}

std::string
TrSys::query_transfer(const std::string &from, const std::string &to, int day,
                      bool tp) { // tp: true for time, false for cost
  struct Ans {
    const Train *from_train = nullptr;
    const Train *to_train = nullptr;
    EveryTr from_tr, to_tr;
    int time = -1, cost = -1;
    int i = -1, j = -1;
    int rank1 = -1, rank2 = -1;

    Ans() {}

    void update_by_time(const Train &_from_train, const Train &_to_train,
                        const EveryTr &_from_tr, const EveryTr &_to_tr,
                        int _time, int _cost, int _i, int _j, int s1, int s2) {
      auto update = [&]() {
        from_train = &_from_train;
        to_train = &_to_train;
        from_tr = _from_tr;
        to_tr = _to_tr;
        time = _time;
        cost = _cost;
        i = _i;
        j = _j;
        rank1 = s1;
        rank2 = s2;
      };

      if (time == -1 && cost == -1) {
        update();
        return;
      }

      if (_time < time) {
        update();
        return;
      } else if (_time == time) {

        if (_cost < cost) {
          update();
          return;
        } else if (_cost == cost) {

          const int res1 = strcmp(_from_tr.id, from_tr.id);
          if (res1 < 0) {
            update();
            return;
          } else if (res1 == 0) {

            const int res2 = strcmp(_to_tr.id, to_tr.id);
            if (res2 < 0) {
              update();
              return;
            }
          }
        }
      }
    }

    void update_by_cost(const Train &_from_train, const Train &_to_train,
                        const EveryTr &_from_tr, const EveryTr &_to_tr,
                        int _time, int _cost, int _i, int _j, int s1, int s2) {
      auto update = [&]() {
        from_train = &_from_train;
        to_train = &_to_train;
        from_tr = _from_tr;
        to_tr = _to_tr;
        time = _time;
        cost = _cost;
        i = _i;
        j = _j;
        rank1 = s1;
        rank2 = s2;
      };

      if (time == -1 && cost == -1) {
        update();
        return;
      }

      if (_cost < cost) {
        update();
        return;
      } else if (_cost == cost) {

        if (_time < time) {
          update();
          return;
        } else if (_time == time) {

          const int res1 = strcmp(_from_tr.id, from_tr.id);
          if (res1 < 0) {
            update();
            return;
          } else if (res1 == 0) {

            const int res2 = strcmp(_to_tr.id, to_tr.id);
            if (res2 < 0) {
              update();
              return;
            }
          }
        }
      }
    }
  } ans;

  const int _from = serials[from], _to = serials[to];
  Station tmp;
  tmp.name = _from;
  auto from_ids = stat_data.find_trains(tmp);
  tmp.name = _to;
  auto to_ids = stat_data.find_trains(tmp);
  Train *const from_trains = new Train[from_ids.size()];
  Train *const to_trains = new Train[to_ids.size()];
  int *const rank_from = new int[from_ids.size()];
  int *const rank_to = new int[to_ids.size()];
  int *const time_from = new int[from_ids.size()];
  int *const time_to = new int[to_ids.size()];
  int *const cost_from = new int[from_ids.size()];
  int *const cost_to = new int[to_ids.size()];

  for (int i = 0; i != from_ids.size(); ++i) {
    strcpy(from_trains[i].id, from_ids[i].c_str());
    train_data.at(from_trains[i]);
    rank_from[i] = get_rank(from_trains[i], _from);
    time_from[i] = total_time(from_trains[i], _from);
    cost_from[i] = total_cost(from_trains[i], _from);
  }
  for (int j = 0; j != to_ids.size(); ++j) {
    strcpy(to_trains[j].id, to_ids[j].c_str());
    train_data.at(to_trains[j]);
    rank_to[j] = get_rank(to_trains[j], _to);
    time_to[j] = total_time(to_trains[j], _to);
    cost_to[j] = total_cost(to_trains[j], _to);
  }

  for (int i = 0; i != from_ids.size(); ++i) {
    for (int j = 0; j != to_ids.size(); ++j) {
      const auto &from_train = from_trains[i];
      const auto &to_train = to_trains[j];
      if (strcmp(from_train.id, to_train.id) == 0) {
        continue;
      }

      const auto ranks =
          is_intersect(from_train, to_train, rank_from[i], rank_to[j]);

      if (ranks.first == -1 && ranks.second == -1) {
        continue;
      }
      int from_day = day;
      Time from_time(day + from_train.start_t + time_from[i]);
      while (from_time.stamp() > day + DAY - 1) {
        from_time -= DAY;
        from_day -= DAY;
      }
      int to_day = from_day;
      EveryTr from_tr, to_tr;
      strcpy(from_tr.id, from_ids[i].c_str());
      from_tr.day = from_day;
      if (!every_train.at(from_tr)) {
        continue;
      }

      const int arrive_trans_time =
          from_day + total_time(from_train, ranks.first);
      int leave_trans_time =
          to_day + total_time(to_train, ranks.second) + to_train.start_t;
      if (ranks.second != 0) {
        leave_trans_time += to_train.stop_t[ranks.second - 1];
      }

      while (leave_trans_time < arrive_trans_time) {
        leave_trans_time += DAY;
        to_day += DAY;
      }
      while (leave_trans_time - DAY >= arrive_trans_time) {
        leave_trans_time -= DAY;
        to_day -= DAY;
      }

      strcpy(to_tr.id, to_ids[j].c_str());
      to_tr.day = to_day;
      if (!every_train.at(to_tr)) {
        continue;
      }

      const int cost = (total_cost(from_train, ranks.first) - cost_from[i]) +
                       (cost_to[j] - total_cost(to_train, ranks.second));
      const int time = (to_day + to_train.start_t + time_to[j]) -
                       (from_day + from_train.start_t + time_from[i] +
                        (i == 0 ? 0 : from_train.stop_t[i - 1]));

      if (tp) { // update by time
        ans.update_by_time(from_train, to_train, from_tr, to_tr, time, cost, i,
                           j, ranks.first, ranks.second);
      } else { // update by cost
        ans.update_by_cost(from_train, to_train, from_tr, to_tr, time, cost, i,
                           j, ranks.first, ranks.second);
      }
    }
  }

  std::string res = "";
  if (ans.time == -1 && ans.cost == -1) {
    res += "0";
  } else {
    const std::string trans = stats[ans.from_train->stations[ans.rank1]];
    res += std::string(ans.from_tr.id) + ' ';
    res += from + ' ';
    const int leave_time1 =
        ans.from_tr.day + ans.from_train->start_t + time_from[ans.i] +
        (ans.i == 0 ? 0 : ans.from_train->stop_t[ans.i - 1]);
    res += Time(leave_time1).display() + " -> ";
    const int arrive_time1 = ans.from_tr.day + ans.from_train->start_t +
                             total_time(*(ans.from_train), ans.rank1);
    res += trans + ' ';
    res += Time(arrive_time1).display() + ' ';
    res += std::to_string(max_seat(ans.from_tr, rank_from[ans.i], ans.rank1)) +
           ' ';
    res += std::to_string(total_cost(*(ans.from_train), ans.rank1) -
                          cost_from[ans.i]) +
           '\n';

    res += std::string(ans.to_tr.id) + ' ';
    res += trans + ' ';
    const int leave_time2 =
        ans.to_tr.day + ans.to_train->start_t +
        total_time(*(ans.to_train), ans.rank2) +
        (ans.rank2 == 0 ? 0 : ans.to_train->stop_t[ans.rank2 - 1]);
    res += Time(leave_time2).display() + " -> ";
    const int arrive_time2 =
        ans.to_tr.day + ans.to_train->start_t + time_to[ans.j];
    res += to + ' ';
    res += Time(arrive_time2).display() + ' ';
    res += std::to_string(max_seat(ans.to_tr, ans.rank2, rank_to[ans.j])) + ' ';
    res += std::to_string(cost_to[ans.j] -
                          total_cost(*(ans.to_train), ans.rank2)) +
           '\n';
  }

  delete[] from_trains;
  delete[] to_trains;
  delete[] rank_from;
  delete[] rank_to;
  delete[] time_from;
  delete[] time_to;
  delete[] cost_from;
  delete[] cost_to;

  return res;
}

int TrSys::buy_ticket(const std::string &usr, const std::string &id, int day,
                      const std::string &from, const std::string &to, int n,
                      bool is_q,
                      int time_stamp) { // -1 for failed, 0 for queue,
                                        // positive for secceeded

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

    History his;
    strcpy(his.usr, usr.c_str());
    strcpy(his.tr_id, id.c_str());
    his.time = time_stamp;
    his.num = n;
    his.start_day = start_day;
    his.from = _from;
    his.leave = time;

    int price = 0;
    bool flag = true;
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
        flag = false;
      }
      price += train.prices[j] * n;
      tr.seat_nums[j] -= n;
    }

    his.to = _to;
    his.arrive = time;
    his.price = price;

    if (flag) {
      every_train.modify(tr);

      his.status = 1;
      history.insert(his);
      // add to user order
      return price;
    } else {

      if (is_q) {
        // add to queue
        // add to user order
        his.status = 0;
        history.insert(his);
        queue.insert(Queue(his));
        return 0;
      } else {
        return -1;
      }
    }

  } else {
    return -1;
  }
}

template <>
vector<History> BPlusTree<History, 123>::query_order(const History &ind) {
  Node node;
  read_node(node, root);
  vector<History> _res;

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
    int res;
    for (; i != node.size; ++i) {
      res = strcmp(ind.usr, node.index[i].usr);
      if (res == 0) {
        // auto &his = node.index[i];
        _res.push_back(node.index[i]);

      } else if (res > 0) {
        break;
      }
    }
    if (res > 0) {
      break;
    }

    if (node.next == INT32_MAX) {
      break;
    }
    read_node(node, node.next);
  }

  return _res;
}

std::string TrSys::query_order(const std::string &usr) {
  if (accounts.usrpriv(usr) == -1) {
    return "";
  }
  History ind;
  strcpy(ind.usr, usr.c_str());
  auto v = history.query_order(ind);
  std::string res = "";
  res += std::to_string(v.size()) + '\n';

  for (int i = 0; i != v.size(); ++i) {
    auto &his = v[i];
    if (his.status == 1) {
      res += "[success] ";
    } else if (his.status == 0) {
      res += "[pending] ";
    } else {                // -1
      res += "[refunded] "; // MHRS
    }
    res += std::string(his.tr_id) + ' ';
    res += stats[his.from] + ' ';
    res += Time(his.leave).display() + " -> ";
    res += stats[his.to] + ' ';
    res += Time(his.arrive).display() + ' ';
    res += std::to_string(his.price) + ' ' + std::to_string(his.num) + '\n';
  }
  return res;
}

bool TrSys::refund_ticket(const std::string &usr, int n) {
  if (accounts.usrpriv(usr) == -1) {
    return false;
  }
  History ind;
  strcpy(ind.usr, usr.c_str());
  auto v = history.query_order(ind);
  if (v.size() < n) {
    return false;
  }
  if (v[n - 1].status == -1) {
    return false;
  }
  if (v[n - 1].status == 1) { // refund
    EveryTr tr;
    Train train;
    strcpy(tr.id, v[n - 1].tr_id);
    strcpy(train.id, v[n - 1].tr_id);
    tr.day = v[n - 1].start_day;
    every_train.at(tr);
    train_data.at(train);

    // restate the train
    auto &his = v[n - 1];
    int i = 0;
    for (; i != train.stat_num; ++i) {
      if (train.stations[i] == his.from) {
        break;
      }
    }

    for (; i != train.stat_num; ++i) {
      if (train.stations[i] == his.to) {
        break;
      }
      tr.seat_nums[i] += his.num;
    }
    every_train.modify(tr);

    //  query the queue

    Queue q;
    queue.traverse(q, true);
    vector<Queue> finished;
    if (queue_ticket(q)) {
      finished.push_back(q);
    }
    while (queue.traverse(q)) {
      if (queue_ticket(q)) {
        finished.push_back(q);
      }
    }

    for (auto it = finished.begin(); it != finished.end(); ++it) {
      queue.erase(*it);
      History tmp(*it);
      history.at(tmp);
      tmp.status = 1;
      history.modify(tmp);
    }
  }
  v[n - 1].status = -1;
  history.modify(v[n - 1]);
  return true;
}

bool TrSys::queue_ticket(const Queue &q) {
  Train train;
  strcpy(train.id, q.tr_id);

  train_data.at(train);
  // const auto _from = serials[from], _to = serials[to];
  EveryTr tr;
  strcpy(tr.id, q.tr_id);

  // Time time(day + train.start_t);
  // int start_day = day;
  int j = 0;
  for (; j != train.stat_num - 1; ++j) {
    if (train.stations[j] == q.from) {
      break;
    }
  }

  tr.day = q.start_day;
  every_train.at(tr);

  bool flag = true;
  for (; j != train.stat_num; ++j) {
    if (train.stations[j] == q.to) {
      break;
    }
    if (j == train.stat_num - 1) {
      break;
    }
    if (tr.seat_nums[j] < q.num) {
      flag = false;
    }
    tr.seat_nums[j] -= q.num;
  }

  if (flag) {
    every_train.modify(tr);
    // add to user order
    return true;
  } else {
    return false;
  }
}

void TrSys::clear() {
  train_data.clear();
  stat_data.clear();
  every_train.clear();
  serials.clear();
  stats.clear();
  history.clear();
  queue.clear();
  max_serial = 0;
}