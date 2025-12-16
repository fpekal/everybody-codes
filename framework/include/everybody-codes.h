#pragma once
#include <functional>
#include <string>

namespace ec {
struct Part {
  std::function<std::string(const std::string &)> impl;

  // TODO: TESTS
};

class Runtime {
public:
  void run();

  Part part1;
  Part part2;
  Part part3;
};
} // namespace ec
