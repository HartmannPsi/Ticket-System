#pragma once
#ifndef STRCHECK_HPP
#define STRCHECK_HPP

#include <string>

class Check {
public:
  static bool username(const std::string &str);

  static bool password(const std::string &str);

  static bool name(const std::string &str);

  static bool mail(const std::string &str);

  static bool trainID(const std::string &str);

  static bool station(const std::string &str);
};

#endif