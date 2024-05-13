#include "time.hpp"

Time::Time(long long stamp) { // stamp = 0, Time = 2024/06/01/00:00

  auto foo = [&]() {
    day = stamp / DAY + 1;
    stamp %= DAY;
    hour = stamp / HOUR;
    stamp %= HOUR;
    min = stamp;
  };

  if (stamp < 0) {
    throw "Time Too Early!";

  } else if (stamp < DAY * 30) { // June
    mon = 6;
    foo();
  } else if (stamp < DAY * (30 + 31)) { // July
    mon = 7;
    stamp -= DAY * 30;
    foo();
  } else if (stamp < DAY * (30 + 31 + 31)) { // August
    mon = 8;
    stamp -= DAY * (30 + 31);
    foo();
  } else if (stamp < DAY * (30 + 31 + 31 + 30)) { // September
    mon = 9;
    stamp -= DAY * (30 + 31 + 31);
    foo();
  } else if (stamp < DAY * (30 + 31 + 31 + 30 + 31)) { // October
    mon = 10;
    stamp -= DAY * (30 + 31 + 31 + 30);
    foo();
  } else if (stamp < DAY * (30 + 31 + 31 + 30 + 31 + 30)) { // November
    mon = 11;
    stamp -= DAY * (30 + 31 + 31 + 30 + 31);
    foo();
  } else if (stamp < DAY * (30 + 31 + 31 + 30 + 31 + 30 + 31)) { // December
    mon = 12;
    stamp -= DAY * (30 + 31 + 31 + 30 + 31 + 30);
    foo();
  } else {
    throw "Time Too Late!";
  }
}

Time &Time::operator+=(const Time &other) {
  long long stamp = (long long)(*this) + (long long)(other);
  *this = Time(stamp);
  return *this;
}

Time &Time::operator+=(long long other) {
  long long stamp = (long long)(*this) + other;
  *this = Time(stamp);
  return *this;
}

Time Time::operator+(const Time &other) const {
  Time tmp(*this);
  tmp += other;
  return tmp;
}

Time Time::operator+(long long other) const {
  Time tmp(*this);
  tmp += other;
  return tmp;
}

bool Time::operator<(const Time &other) const {
  return (long long)(*this) < (long long)(other);
}

bool Time::operator<=(const Time &other) const {
  return (long long)(*this) <= (long long)(other);
}

bool Time::operator>(const Time &other) const {
  return (long long)(*this) > (long long)(other);
}

bool Time::operator>=(const Time &other) const {
  return (long long)(*this) >= (long long)(other);
}

bool Time::operator==(const Time &other) const {
  return (long long)(*this) == (long long)(other);
}

Time::operator long long() const {
  long long res = 0;
  res += min;
  res += hour * HOUR;
  res += (day - 1) * DAY;
  switch (mon) {
  case 12:
    res += 30 * DAY;
  case 11:
    res += 31 * DAY;
  case 10:
    res += 30 * DAY;
  case 9:
    res += 31 * DAY;
  case 8:
    res += 31 * DAY;
  case 7:
    res += 30 * DAY;
  case 6:
  default:
    break;
  }

  return res;
}

long long Time::stamp() const { return (long long)(*this); }