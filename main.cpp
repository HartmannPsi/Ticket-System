#include "headers.hpp"
#include <iostream>
#include <string>

#define DEBUG

AcSys accounts;
TrSys trains;

int main() {
#ifdef DEBUG
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
#endif

  while (true) {
    std::string input;
    std::getline(std::cin, input);

    try {
      process(input);
    } catch (const std::string &str) {
      std::cout << "[Error] " << str;
    } catch (const char *const str) {
      std::cout << "[Error] " << str;
    } catch (...) {
      std::cout << "[Error] Unkown Exception.\n";
    }
  }

  return 0;
}