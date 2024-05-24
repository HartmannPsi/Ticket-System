#pragma GCC optimize(3)
//#pragma GCC optimize(3, "Ofast", "inline")
#include "headers.hpp"
#include <iostream>
#include <string>

//#define DEBUG

AcSys accounts;
TrSys trains;
int TIME;

int main() {
#ifndef DEBUG
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
#endif

  while (true) {
    std::string input;
    std::getline(std::cin, input);

    try {
      process(input);
    }
#ifdef DEBUG
    catch (const std::string &str) {
      std::cout << "[Error] " << str;
    } catch (const char *const str) {
      std::cout << "[Error] " << str;
    } catch (...) {
      std::cout << "[Error] Unknown Exception.\n";
    }
#endif
#ifndef DEBUG
    catch (...) {
      std::cout << "-1\n";
    }

#endif
  }

  return 0;
}