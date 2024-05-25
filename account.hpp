#pragma once
#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "base.hpp"
#include "bpt.hpp"
#include <iostream>

struct Account { // M = 73
  char username[21] = {};
  char password[31] = {};
  char name[16] = {};
  char mail[31] = {};
  int privilege = 0;

  friend std::ostream &operator<<(std::ostream &os, const Account &obj) {
    os << obj.username << " -p " << obj.password << " -n " << obj.name << " -m "
       << obj.mail << " -g " << obj.privilege;
    return os;
  }

  Account(const std::string &usr, const std::string &psw, const std::string &n,
          const std::string &m, int k)
      : privilege(k) {
    strcpy(username, usr.c_str());
    strcpy(password, psw.c_str());
    strcpy(name, n.c_str());
    strcpy(mail, m.c_str());
  }

  Account(const std::string &usr) { strcpy(username, usr.c_str()); }

  Account() {}

  ~Account() = default;

  Account(const Account &other) {
    privilege = other.privilege;
    strcpy(username, other.username);
    strcpy(password, other.password);
    strcpy(name, other.name);
    strcpy(mail, other.mail);
  }

  Account &operator=(const Account &other) {
    if (&other == this) {
      return *this;
    }

    privilege = other.privilege;
    strcpy(username, other.username);
    strcpy(password, other.password);
    strcpy(name, other.name);
    strcpy(mail, other.mail);

    return *this;
  }

  bool operator==(const Account &other) const {
    const int res = strcmp(username, other.username);
    return res == 0;
  }
  bool operator<(const Account &other) const {
    const int res = strcmp(username, other.username);
    return res < 0;
  }
  bool operator>(const Account &other) const {
    const int res = strcmp(username, other.username);
    return res > 0;
  }
  bool operator<=(const Account &other) const {
    const int res = strcmp(username, other.username);
    return res <= 0;
  }
  bool operator>=(const Account &other) const {
    const int res = strcmp(username, other.username);
    return res >= 0;
  }
};

template <> class Base<Account> {
  BPlusTree<Index, 101> index;
  std::fstream data;
  const std::string file_name;
  u32 end;

  // using Val = Account;

  void read(Account &dest, u32 pos) {
    data.seekg(pos);
    data.read(reinterpret_cast<char *>(&dest), sizeof(Account));
  }

  void write(const Account &src, u32 pos) {
    data.seekp(pos);
    data.write(reinterpret_cast<const char *>(&src), sizeof(Account));
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

  bool insert(const Account &val) {
    Index ind({val.username, end});

    if (index.at(ind)) {
      return false;
    }

    if (index.insert(ind)) {
      write(val, end);
      end += sizeof(Account);
      return true;
    } else {
      return false;
    }
  }

  u32 get_pos(const Account &val) {
    Index ind(val.username, INT32_MAX);
    index.at(ind);
    return ind.pos;
  }

  bool at(Account &val, u32 pos) {
    read(val, pos);
    return true;
  }

  bool erase(const Account &val) {
    Index ind(val.username, 0);

    // if (TIME == 199054) {
    //   std::cout << ind.str << ' ' << ind.pos << '\n';
    // }

    return index.erase(ind);
  }

  bool modify(const Account &val) {
    Index ind(val.username, 0);

    if (!index.at(ind)) {
      return false;
    }

    write(val, ind.pos);
    return true;
  }

  bool at(Account &dest) {

    Index ind(dest.username, 0);
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

class AcSys {
  Base<Account> data;              // it should be 73
  map<std::string, int> log_table; // string: username, int: privilege

public:
  AcSys();

  ~AcSys();

  bool adduser(const std::string &usr, const std::string &psw,
               const std::string &name, const std::string &mail,
               const std::string &cur_usr, int priv);

  bool login(const std::string &usr, const std::string &psw);

  bool logout(const std::string &usr);

  std::string
  query(const std::string &cur_usr,
        const std::string &usr); // "" if failed, a string if succeeded

  std::string modify(const std::string &usr, const std::string &psw,
                     const std::string &name, const std::string &mail,
                     const std::string &cur_usr,
                     int priv); // string: "" for default, int: -1 for default

  int usrpriv(const std::string &usr) const; // 0-10 if logged in, -1 if not

  void clear();

  // void traverse() { data.traverse(); }
};

#endif