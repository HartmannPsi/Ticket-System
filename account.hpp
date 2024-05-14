#pragma once
#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "bpt.hpp"

struct Account { // M = 73
  char username[21] = {};
  char password[31] = {};
  char name[16] = {};
  char mail[31] = {};
  int privilege = 1;

  Account(const std::string &usr, const std::string &psw, const std::string &n,
          const std::string &m, int k)
      : privilege(k) {
    strcpy(username, usr.c_str());
    strcpy(password, psw.c_str());
    strcpy(name, n.c_str());
    strcpy(mail, m.c_str());
  }

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

constexpr int M = 203; // M should be odd (101)
constexpr u32 BLOCK = 4096 * 2;
constexpr int CACHE_SIZE = 2048;

struct Node {
  Index index[M];
  u32 child[M];
  int size = 0;
  bool is_leaf = false;
  u32 next = 0;

  Node() {}
  ~Node() {}
};

constexpr u32 BSIZE = sizeof(Node);

// BPlusTree<Account, 73> accounts("accounts.dat", "accounts.rec",
//                                 "accounts.root");

#endif