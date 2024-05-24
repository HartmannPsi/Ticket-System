#pragma GCC optimize(3)
#include "input_process.hpp"

//#define TEST

extern AcSys accounts;
extern TrSys trains;
extern int TIME;

std::string slice(const std::string &_str) {
  static int pos = 0;
  static std::string str = "";

  if (_str == "") {
    if (pos == std::string::npos || str == "") {
      return "";
    }

    int tmp = pos + 1;
    while (str[tmp] == ' ') {
      ++tmp;
    }

    pos = str.find(' ', tmp);
    return str.substr(tmp, pos - tmp);
  } else {
    pos = 0;
    str = _str;
    pos = str.find(' ', pos);
    return str.substr(0, pos);
  }
}

void to_int_array(const std::string &str, int size, int *array) {

  auto slice = [](const std::string &_str = "") {
    static int pos = 0;
    static std::string str = "";

    if (_str == "") {
      if (pos == std::string::npos || str == "") {
        return std::string("");
      }

      const int tmp = pos + 1;
      pos = str.find('|', tmp);
      return str.substr(tmp, pos - tmp);
    } else {
      pos = 0;
      str = _str;
      pos = str.find('|', pos);
      return str.substr(0, pos);
    }
  };

  array[0] = std::stoi(slice(str));
  for (int i = 1; i != size; ++i) {
    array[i] = std::stoi(slice());
  }
}

void to_str_array(const std::string &str, int size, std::string *array) {

  auto slice = [](const std::string &_str = "") {
    static int pos = 0;
    static std::string str = "";

    if (_str == "") {
      if (pos == std::string::npos || str == "") {
        return std::string("");
      }

      const int tmp = pos + 1;
      pos = str.find('|', tmp);
      return str.substr(tmp, pos - tmp);
    } else {
      pos = 0;
      str = _str;
      pos = str.find('|', pos);
      return str.substr(0, pos);
    }
  };

  array[0] = slice(str);
  for (int i = 1; i != size; ++i) {
    array[i] = slice();
  }
}

void process(const std::string &input) {
  const std::string time_stamp = slice(input);
  const std::string cmd = slice();

  TIME = std::stoi(time_stamp.substr(1, time_stamp.size() - 2));

  std::cout << time_stamp << ' ';

  // if (TIME == 3524) {
  //   trains.traverse_everytr();
  // }

  if (cmd == "add_user") {

    std::string cur_usr = "", usr = "", psw = "", name = "", mail = "";
    int priv = -1;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-c") {
        cur_usr = slice();

      } else if (tp == "-u") {
        usr = slice();

      } else if (tp == "-p") {
        psw = slice();

      } else if (tp == "-n") {
        name = slice();

      } else if (tp == "-m") {
        mail = slice();

      } else { // "-g"
        priv = std::stoi(slice());
      }

      return true;
    };

    while (get_info())
      ;

    if (accounts.adduser(usr, psw, name, mail, cur_usr, priv)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to add user.\n";
    }

  } else if (cmd == "login") {

    std::string usr = "", psw = "";

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-u") {
        usr = slice();

      } else { // "-p"
        psw = slice();
      }

      return true;
    };

    while (get_info())
      ;

    if (accounts.login(usr, psw)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to login.\n";
    }

  } else if (cmd == "logout") {

    std::string usr = "";
    slice();
    usr = slice();

    if (accounts.logout(usr)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to logout.\n";
    }

  } else if (cmd == "query_profile") {

    std::string usr = "", cur_usr = "";

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-u") {
        usr = slice();

      } else { // "-c"
        cur_usr = slice();
      }

      return true;
    };

    while (get_info())
      ;

    const auto res = accounts.query(cur_usr, usr);
    if (res == "") {
      throw "-1: Failed to query profile.\n";
    } else {
      std::cout << res << '\n';
    }

  } else if (cmd == "modify_profile") {

    std::string cur_usr = "", usr = "", psw = "", name = "", mail = "";
    int priv = -1;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-c") {
        cur_usr = slice();

      } else if (tp == "-u") {
        usr = slice();

      } else if (tp == "-p") {
        psw = slice();

      } else if (tp == "-n") {
        name = slice();

      } else if (tp == "-m") {
        mail = slice();

      } else { // "-g"
        priv = std::stoi(slice());
      }

      return true;
    };

    while (get_info())
      ;

    const auto res = accounts.modify(usr, psw, name, mail, cur_usr, priv);

    if (res == "") {
      throw "-1: Failed to modify profile.\n";
    } else {
      std::cout << res << '\n'; // Hartmann_Psi
    }

  } else if (cmd == "add_train") {

    std::string id = "", stations = "", prices = "", travel_t = "", stop_t = "",
                sale_d = "";
    int stat_num = -1, seat_num = -1;
    char type = 'A';
    Time start_t(0, 0), start_sale(0, 0), end_sale(0, 0);

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-i") {
        id = slice();

      } else if (tp == "-n") {
        stat_num = std::stoi(slice());

      } else if (tp == "-m") {
        seat_num = std::stoi(slice());

      } else if (tp == "-s") {
        stations = slice();

      } else if (tp == "-p") {
        prices = slice();

      } else if (tp == "-x") {
        const std::string tmp = slice();
        start_t.hour = int8_t(std::stoi(tmp.substr(0, 2)));
        start_t.min = int8_t(std::stoi(tmp.substr(3, 2)));

      } else if (tp == "-t") {
        travel_t = slice();

      } else if (tp == "-o") {
        stop_t = slice();

      } else if (tp == "-d") {
        const std::string tmp = slice();
        start_sale.mon = int8_t(std::stoi(tmp.substr(0, 2)));
        start_sale.day = int8_t(std::stoi(tmp.substr(3, 2)));
        end_sale.mon = int8_t(std::stoi(tmp.substr(6, 2)));
        end_sale.day = int8_t(std::stoi(tmp.substr(9, 2)));

      } else { // "-y"
        type = slice()[0];
      }

      return true;
    };

    while (get_info())
      ;

    int *const _prices = new int[stat_num - 1];
    int *const _travel_t = new int[stat_num - 1];
    int *_stop_t = nullptr;
    std::string *const _stations = new std::string[stat_num];

    to_int_array(prices, stat_num - 1, _prices);
    to_int_array(travel_t, stat_num - 1, _travel_t);
    to_str_array(stations, stat_num, _stations);
    if (stat_num != 2) {
      _stop_t = new int[stat_num - 2];
      to_int_array(stop_t, stat_num - 2, _stop_t);
    }

    if (trains.add_train(id, stat_num, seat_num, _stations, _prices,
                         int(start_t), _travel_t, _stop_t, int(start_sale),
                         int(end_sale), type)) {
      std::cout << "0\n";
      delete[] _prices;
      delete[] _travel_t;
      delete[] _stop_t;
      delete[] _stations;
    } else {
      delete[] _prices;
      delete[] _travel_t;
      delete[] _stop_t;
      delete[] _stations;
      throw "-1: Failed to add train.\n";
    }

    // delete[] _prices;
    // delete[] _travel_t;
    // delete[] _stop_t;
    // delete[] _stations;

  } else if (cmd == "delete_train") {

    std::string id = "";

    slice();
    id = slice();

    if (trains.delete_train(id)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to delete train.\n";
    }

  } else if (cmd == "release_train") {

    std::string id = "";

    slice();
    id = slice();

    if (trains.release_train(id)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to release train.\n";
    }

  } else if (cmd == "query_train") {

    std::string id = "";
    Time day(0);

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-i") {
        id = slice();
      } else { // "-d"
        const std::string tmp = slice();
        day.mon = int8_t(std::stoi(tmp.substr(0, 2)));
        day.day = int8_t(std::stoi(tmp.substr(3, 2)));
      }

      return true;
    };

    while (get_info())
      ;

    if (day.mon < 6) {
      throw "无效的时间\n";
    }

    const auto res = trains.query_train(id, int(day));
    if (res == "") {
      throw "-1: Failed to query train.\n";
    } else {
      std::cout << res;
    }

  } else if (cmd == "query_ticket") {
    std::string from = "", to = "";
    Time day(0);
    bool _tp = true;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-s") {
        from = slice();

      } else if (tp == "-t") {
        to = slice();

      } else if (tp == "-d") {
        const std::string tmp = slice();
        day.mon = int8_t(std::stoi(tmp.substr(0, 2)));
        day.day = int8_t(std::stoi(tmp.substr(3, 2)));

      } else { // "-p"
        _tp = (slice() == "time");
      }

      return true;
    };

    while (get_info())
      ;

    if (day.mon < 6) {
      std::cout << "0\n";
      return;
    }

    std::cout << trains.query_ticket(from, to, int(day), _tp);

  } else if (cmd == "query_transfer") {

#ifdef TEST
    std::cout << '\n';
    return;
#endif

    std::string from = "", to = "";
    Time day(0);
    bool _tp = true;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-s") {
        from = slice();

      } else if (tp == "-t") {
        to = slice();

      } else if (tp == "-d") {
        const std::string tmp = slice();
        day.mon = int8_t(std::stoi(tmp.substr(0, 2)));
        day.day = int8_t(std::stoi(tmp.substr(3, 2)));

      } else { // "-p"
        _tp = (slice() == "time");
      }

      return true;
    };

    while (get_info())
      ;

    if (day.mon < 6) {
      std::cout << "0\n";
      return;
    }

    std::cout << trains.query_transfer(from, to, int(day), _tp);

  } else if (cmd == "buy_ticket") {

#ifdef TEST
    std::cout << '\n';
    return;
#endif

    std::string usr = "", id = "", from = "", to = "";
    Time day(0);
    int n = -1;
    bool is_q = false;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-u") {
        usr = slice();

      } else if (tp == "-i") {
        id = slice();

      } else if (tp == "-d") {
        const std::string tmp = slice();
        day.mon = int8_t(std::stoi(tmp.substr(0, 2)));
        day.day = int8_t(std::stoi(tmp.substr(3, 2)));

      } else if (tp == "-n") {
        n = std::stoi(slice());

      } else if (tp == "-f") {
        from = slice();

      } else if (tp == "-t") {
        to = slice();

      } else { // "-q"
        is_q = (slice() == "true");
      }

      return true;
    };

    while (get_info())
      ;

    if (day.mon < 6) {
      throw "无效的时间\n";
    }

    int time = std::stoi(time_stamp.substr(1, time_stamp.size() - 2));

    const auto res =
        trains.buy_ticket(usr, id, int(day), from, to, n, is_q, time);

    if (res > 0) {
      std::cout << res << '\n';
    } else if (res == 0) {
      std::cout << "queue\n";
    } else { // res == -1
      throw "-1: Failed to buy ticket.\n";
    }

  } else if (cmd == "query_order") {

#ifdef TEST
    std::cout << '\n';
    return;
#endif

    std::string usr = "";
    slice();
    usr = slice();

    const auto res = trains.query_order(usr);

    if (res == "") {
      throw "-1: Failed to query order.\n";
    } else {
      std::cout << res;
    }

  } else if (cmd == "refund_ticket") {

#ifdef TEST
    std::cout << '\n';
    return;
#endif

    std::string usr = "";
    int n = 1;

    auto get_info = [&]() {
      const std::string tp = slice();

      if (tp == "") {
        return false;
      }

      if (tp == "-u") {
        usr = slice();
      } else { // "-n"
        n = std::stoi(slice());
      }

      return true;
    };

    while (get_info())
      ;

    if (trains.refund_ticket(usr, n)) {
      std::cout << "0\n";
    } else {
      throw "-1: Failed to refund ticket.\n";
    }

  } else if (cmd == "clean") {

    accounts.clear();
    trains.clear();
    std::cout << "0\n";

  } else if (cmd == "exit") {

    std::cout << "bye\n";
    exit(0);

  } else {
    std::cout << "BabaBoi\n"
                 "Hey boy, you got the wrong door!\n";
  }
}