#pragma once
#include <functional>
#include <string>

namespace ec {
struct Input {
  std::string filename;
  std::string expected_result;
};

struct Part {
  std::function<std::string(const std::string &)> impl;

  Input input;
  std::vector<Input> tests;
};

class Runtime {
public:
  Runtime(int argc, char *argv[]);

  bool run();

  Part part1;
  Part part2;
  Part part3;

  int non_interactive_part = 0;
  int non_interactive_test_to_run = -1;
  bool non_interactive_run_tests = false;
  bool interactive = true;
  bool do_download_inputs = false;
};
} // namespace ec
