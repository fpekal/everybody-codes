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
  Runtime(int argc, char *argv[]);

  void run();

  Part part1;
  Part part2;
  Part part3;

  int non_interactive_part = 0;
  bool interactive = true;
};
} // namespace ec
