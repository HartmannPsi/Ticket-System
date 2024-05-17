#include "time.hpp"
#include <string>

Time::Time(int stamp) { // stamp = 0, Time = 2024/06/01/00:00

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
  int stamp = (int)(*this) + (int)(other);
  *this = Time(stamp);
  return *this;
}

Time &Time::operator+=(int other) {
  int stamp = (int)(*this) + other;
  *this = Time(stamp);
  return *this;
}

Time Time::operator+(const Time &other) const {
  Time tmp(*this);
  tmp += other;
  return tmp;
}

Time Time::operator+(int other) const {
  Time tmp(*this);
  tmp += other;
  return tmp;
}

bool Time::operator<(const Time &other) const {
  return (int)(*this) < (int)(other);
}

bool Time::operator<=(const Time &other) const {
  return (int)(*this) <= (int)(other);
}

bool Time::operator>(const Time &other) const {
  return (int)(*this) > (int)(other);
}

bool Time::operator>=(const Time &other) const {
  return (int)(*this) >= (int)(other);
}

bool Time::operator==(const Time &other) const {
  return (int)(*this) == (int)(other);
}

Time::operator int() const {
  int res = 0;
  res += int(min);
  res += int(hour) * HOUR;
  res += int(day - 1) * DAY;
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

int Time::stamp() const { return (int)(*this); }

std::string Time::display() const {
  std::string res = "";
  if (mon < 10) {
    res += '0';
  }
  res += std::to_string(mon);
  res += '-';
  if (day < 10) {
    res += '0';
  }
  res += std::to_string(day);
  res += ' ';
  if (hour < 10) {
    res += '0';
  }
  res += std::to_string(hour);
  res += ':';
  if (min < 10) {
    res += '0';
  }
  res += std::to_string(min);

  return res;
}