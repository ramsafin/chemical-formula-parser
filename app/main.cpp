#include <iostream>

#include "cfp/library.hpp"

int main() {
  const int LEFT  = 10;
  const int RIGHT = 4;

  std::cout << "Add: " << cfp::add(LEFT, RIGHT) << '\n';
  std::cout << "Subtract: " << cfp::subtract(LEFT, RIGHT) << '\n';

  return 0;
}
