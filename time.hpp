#pragma once
#ifndef TIME_HPP
#define TIME_HPP

#include <cstdint>
#include <string>

constexpr int HOUR = 60, DAY = 24 * HOUR,
              MAX_TIME = DAY * (30 + 31 + 31 + 30 + 31 + 30 + 31);

class Time {
public:
  int8_t min;
  int8_t hour;
  int8_t day;
  int8_t mon;

  Time(int _mon, int _day, int _hour, int _min)
      : mon(_mon), day(_day), hour(_hour), min(_min) {}

  Time(int _hour, int _min) : mon(0), day(0), hour(_hour), min(_min) {}

  Time(int stamp); // stamp = 0, Time = 2024/06/01/00:00

  ~Time() = default;

  Time(const Time &other) = default;

  Time(Time &&other) = default;

  Time &operator=(const Time &other) = default;

  Time &operator=(Time &&other) = default;

  Time &operator+=(const Time &other);
  Time &operator+=(int other);

  Time operator+(const Time &other) const;
  Time operator+(int other) const;

  bool operator<(const Time &other) const;
  bool operator<=(const Time &other) const;
  bool operator>(const Time &other) const;
  bool operator>=(const Time &other) const;
  bool operator==(const Time &other) const;

  operator int() const;
  int stamp() const;

  std::string display() const;
};

#endif