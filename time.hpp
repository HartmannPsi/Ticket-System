#pragma once
#ifndef TIME_HPP
#define TIME_HPP

#include <stdint.h>

constexpr long long HOUR = 60, DAY = 24 * HOUR;

class Time {
public:
  int8_t min;
  int8_t hour;
  int8_t day;
  int8_t mon;

  Time(int _mon, int _day, int _hour, int _min)
      : mon(_mon), day(_day), hour(_hour), min(_min) {}

  Time(int _hour, int _min) : mon(0), day(0), hour(_hour), min(_min) {}

  Time(long long stamp); // stamp = 0, Time = 2024/06/01/00:00

  ~Time() = default;

  Time(const Time &other) = default;

  Time(Time &&other) = default;

  Time &operator=(const Time &other) = default;

  Time &operator=(Time &&other) = default;

  Time &operator+=(const Time &other);
  Time &operator+=(long long other);

  Time operator+(const Time &other) const;
  Time operator+(long long other) const;

  bool operator<(const Time &other) const;
  bool operator<=(const Time &other) const;
  bool operator>(const Time &other) const;
  bool operator>=(const Time &other) const;
  bool operator==(const Time &other) const;

  operator long long() const;
  long long stamp() const;
};

#endif