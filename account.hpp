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

// BPlusTree<Account, 73> accounts("accounts.dat", "accounts.rec",
//                                 "accounts.root");

class AcSys {
  BPlusTree<Account, 35> data;     // it should be 73
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

  void traverse() { data.traverse(); }
};

#endif