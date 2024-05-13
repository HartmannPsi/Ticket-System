#include "strcheck.hpp"

bool Check::username(const std::string &str) {

  auto check = [](char ch) {
    if (ch >= '0' && ch <= '9') {
      return true;
    } else if (ch >= 'a' && ch <= 'z') {
      return true;
    } else if (ch >= 'A' && ch <= 'Z') {
      return true;
    } else if (ch == '_') {
      return true;
    } else {
      return false;
    }
  };

  if (str.size() > 20 || str.empty()) {
    return false;
  }

  if (!((str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z'))) {
    return false;
  }

  for (int i = 1; i != str.size(); ++i) {
    if (!check(str[i])) {
      return false;
    }
  }

  return true;
}

bool Check::password(const std::string &str) {

  auto check = [](char ch) { return ch > 32 && ch != 127; };

  if (str.size() > 30 || str.empty()) {
    return false;
  }

  for (int i = 0; i != str.size(); ++i) {
    if (!check(str[i])) {
      return false;
    }
  }

  return true;
}

bool Check::name(const std::string &str) {}

bool Check::mail(const std::string &str) {
  auto check = [](char ch) {
    if (ch >= '0' && ch <= '9') {
      return true;
    } else if (ch >= 'a' && ch <= 'z') {
      return true;
    } else if (ch >= 'A' && ch <= 'Z') {
      return true;
    } else if (ch == '@' || ch == '.') {
      return true;
    } else {
      return false;
    }
  };

  if (str.size() > 30 || str.empty()) {
    return false;
  }

  for (int i = 0; i != str.size(); ++i) {
    if (!check(str[i])) {
      return false;
    }
  }

  return true;
}

bool Check::trainID(const std::string &str) { return username(str); }

bool Check::station(const std::string &str) {}