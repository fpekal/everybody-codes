#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <print>
#include <queue>
#include <ratio>
#include <stdexcept>
#include <termios.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "../include/everybody-codes.h"

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#ifndef UTILS_DIR
#define UTILS_DIR /
#endif
#define UTILS_DIR_STR STR(UTILS_DIR)

static bool has_part_n(const ec::Runtime &rt, int n) {
  if (n == 1) {
    return (bool)rt.part1.impl;
  } else if (n == 2) {
    return (bool)rt.part2.impl;
  } else if (n == 3) {
    return (bool)rt.part3.impl;
  }

  return false;
}

#define CSI "\x1B["

static constexpr std::string ansi_color(int col) {
  return std::string{CSI "38;5;"} + std::to_string(col) + 'm';
}

static constexpr std::string ansi_bold() { return CSI "1m"; }
static constexpr std::string ansi_reset() { return CSI "0m"; }

static constexpr std::string ansi_to_left_column() { return "\r"; }
static constexpr std::string ansi_up_n(int n) {
  return std::string{CSI} + std::to_string(n) + 'A';
}

class TermRawMode {
public:
  TermRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  }

  ~TermRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

private:
  termios orig_termios;
};

struct Experiment {
  std::string last_return_value = "";
  std::chrono::milliseconds duration;
  bool running = false;
};

std::mutex input_queue_mutex;
std::condition_variable input_queue_cv;
std::queue<char> input_queue;

std::mutex task_queue_mutex;
std::condition_variable task_queue_cv;
std::queue<int> task_queue;

std::mutex experiments_mutex;
std::array<Experiment, 3> experiments;

static const ec::Part &get_part_n(const ec::Runtime &rt, int n) {
  if (n == 1) {
    return rt.part1;
  } else if (n == 2) {
    return rt.part2;
  }

  return rt.part3;
}

static void task_thread(const ec::Runtime &rt) {
  while (true) {
    std::unique_lock lock{task_queue_mutex};
    task_queue_cv.wait(lock, [] { return task_queue.size() > 0; });

    while (true) {
      if (task_queue.size() <= 0)
        break;

      int exp_num = task_queue.front();
      task_queue.pop();
      if (!get_part_n(rt, exp_num).impl)
        continue;
      lock.unlock();

      {
        std::unique_lock lock{experiments_mutex};
        experiments.at(exp_num - 1).running = true;
      }

      auto &part = get_part_n(rt, exp_num);
      auto start = std::chrono::high_resolution_clock::now();
      std::string value = part.impl(part.input.filename);
      auto end = std::chrono::high_resolution_clock::now();

      {
        std::unique_lock lock{experiments_mutex};
        experiments.at(exp_num - 1).running = false;
        experiments.at(exp_num - 1).last_return_value = value;
        experiments.at(exp_num - 1).duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      }

      lock.lock();
    }
  }
}

static void input_thread() {
  TermRawMode trm;

  while (true) {
    char c;
    std::cin.read(&c, 1);

    std::unique_lock lock{input_queue_mutex};
    input_queue.push(c);
    input_queue_cv.notify_one();
  }
}

int menu_pos = 0;

static void draw(const ec::Runtime &rt) {
  for (int i = 0; i < 3; ++i) {
    if (menu_pos == i) {
      std::print("> ");
    } else {
      std::print("  ");
    }

    if (!has_part_n(rt, i + 1)) {
      std::print("{}", ansi_color(242));
    } else {
      std::unique_lock lock{experiments_mutex};
      if (experiments.at(i).running)
        std::print("{}", ansi_color(202));
    }
    std::print("Part{}", i + 1);
    {
      std::unique_lock lock{experiments_mutex};
      if (experiments.at(i).last_return_value != "") {
        std::print("{} -> {}{}{} - {}{}ms{}", ansi_reset(), ansi_color(123),
                   experiments.at(i).last_return_value, ansi_reset(),
                   ansi_color(242), experiments.at(i).duration.count(),
                   ansi_reset());
      }
    }
    std::println();
    std::print("{}", ansi_reset());
  }

  std::print("{}", ansi_to_left_column());
  std::print("{}", ansi_up_n(3));
}

ec::Runtime::Runtime(int argc, char *argv[]) {
  argc -= 1;
  argv += 1;

  while (argc > 0) {
    std::string arg = argv[0];

    if (arg == "--interactive" || arg == "-i") {
      interactive = true;
      argc -= 1;
      argv += 1;
      continue;
    } else if (arg == "--noninteractive" || arg == "-I") {
      interactive = false;
      argc -= 1;
      argv += 1;
      continue;
    } else if (arg == "--part" || arg == "-p") {
      if (argc < 2) {
        std::println("ERROR: {} expects a number", arg);
        break;
      }

      std::string number_str = argv[1];
      try {
        int number = std::stoi(number_str);
        if (number > 3 || number < 1) {
          std::println("ERROR: part number should be between 1 and 3, not {}",
                       number);
          break;
        }
        non_interactive_part = number;
        interactive = false;
      } catch (std::logic_error e) {
        std::println("ERROR: expected a number after {}, not {}", arg,
                     number_str);
        break;
      }

      argc -= 2;
      argv += 2;
      continue;
    } else if (arg == "--runtests" || arg == "-t") {
      non_interactive_run_tests = true;
      argc -= 1;
      argv += 1;
      continue;
    } else if (arg == "--test" || arg == "-T") {
      if (argc < 2) {
        std::println("ERROR: {} expects a number", arg);
        break;
      }

      std::string number_str = argv[1];
      try {
        int number = std::stoi(number_str);
        if (number < 0) {
          std::println(
              "ERROR: test number should be greater or equal to 0, not {}",
              number);
          break;
        }
        non_interactive_test_to_run = number;
        non_interactive_run_tests = true;
      } catch (std::logic_error e) {
        std::println("ERROR: expected a number after {}, not {}", arg,
                     number_str);
        break;
      }

      argc -= 2;
      argv += 2;
      continue;
    } else if (arg == "--download-inputs") {
      do_download_inputs = true;
      argc -= 1;
      argv += 1;
      continue;
    }

    // Unknown argument
    std::println("ERROR: Unknown argument: {}", arg);
    break;
  }
}

static std::pair<std::string, long long> run(const ec::Part &part,
                                             const ec::Input &input) {
  auto begin_time = std::chrono::high_resolution_clock::now();
  auto result = part.impl(input.filename);
  auto end_time = std::chrono::high_resolution_clock::now();

  auto duration = end_time - begin_time;
  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  return {result, milliseconds};
}

static void run_interactive(const ec::Runtime &rt) {
  std::jthread input{input_thread};
  std::jthread task{task_thread, std::ref(rt)};

  while (true) {
    std::unique_lock lock{input_queue_mutex};
    input_queue_cv.wait_for(lock, std::chrono::milliseconds{1000 / 30},
                            [] { return input_queue.size() > 0; });

    while (true) {
      if (input_queue.size() <= 0)
        break;

      char c;
      c = input_queue.front();
      input_queue.pop();

      lock.unlock();

      if (c == 'j') {
        menu_pos++;
        if (menu_pos >= 3)
          menu_pos = 2;
      } else if (c == 'k') {
        menu_pos--;
        if (menu_pos < 0)
          menu_pos = 0;
      } else if (c == ' ') {
        {
          std::unique_lock lock{task_queue_mutex};

          task_queue.push(menu_pos + 1);
          task_queue_cv.notify_one();
        }
      }

      lock.lock();
    }

    draw(rt);
  }
}

static void run_tests(const ec::Runtime &rt, const ec::Part &part) {
  if (rt.non_interactive_test_to_run == -1) { // Run all tests
    if (part.tests.size() == 0) {
      std::println("WARNING: no tests");
    }

    for (const auto &test : part.tests) {
      auto [result, milliseconds] = run(part, test);

      bool good = result == test.expected_result;

      if (good) {
        std::print("{}✔️{} ", ansi_color(2), ansi_reset());
      } else {
        std::print("{}{}X {} ", ansi_color(1), ansi_bold(), ansi_reset());
      }

      std::print("{} -> {}{}{} - {}{}ms{}", test.filename, ansi_color(123),
                 result, ansi_reset(), ansi_color(242), milliseconds,
                 ansi_reset());

      if (!good) {
        std::print(" (expected {})", test.expected_result);
      }

      std::println();
    }
  } else {
    if (part.tests.size() == 0) {
      std::println("ERROR: no tests");
      return;
    }
    if (rt.non_interactive_test_to_run > part.tests.size() - 1) {
      std::println(
          "ERROR: test number should be less than tests count ({}), not {}",
          part.tests.size(), rt.non_interactive_test_to_run);
      return;
    }

    auto &test = part.tests.at(rt.non_interactive_test_to_run);
    auto [result, milliseconds] = run(part, test);

    bool good = result == test.expected_result;

    if (good) {
      std::print("{}✔️{} ", ansi_color(2), ansi_reset());
    } else {
      std::print("{}{}X {} ", ansi_color(1), ansi_bold(), ansi_reset());
    }

    std::print("{} -> {}{}{} - {}{}ms{}", test.filename, ansi_color(123),
               result, ansi_reset(), ansi_color(242), milliseconds,
               ansi_reset());

    if (!good) {
      std::print(" (expected {})", test.expected_result);
    }

    std::println();
  }
}

static void run_noninteractive(const ec::Runtime &rt) {
  if (rt.non_interactive_part == 0) {
    std::println("ERROR: part number not set");
    return;
  }

  auto &part = get_part_n(rt, rt.non_interactive_part);

  if (!part.impl) {
    std::println("ERROR: part {} is not defined", rt.non_interactive_part);
    return;
  }

  if (rt.non_interactive_run_tests) {
    run_tests(rt, part);
    return;
  }

  auto [result, milliseconds] = run(part, part.input);

  std::println("{}{}{} - {}{}ms{}", ansi_color(123), result, ansi_reset(),
               ansi_color(242), milliseconds, ansi_reset());
}

static std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;

  auto pipe = popen(cmd, "r"); // get rid of shared_ptr

  if (!pipe)
    throw std::runtime_error("popen() failed!");

  while (!feof(pipe)) {
    if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
      result += buffer.data();
  }

  auto rc = pclose(pipe);

  return result;
}

static void download_inputs() {
  std::ignore = std::system("mkdir inputs 2>/dev/null");

  std::string output = exec(UTILS_DIR_STR "/get_inputs.sh"
                                          " 2>/dev/null");

  auto json = nlohmann::json::parse(output);

  std::string part2 = json["2"].get<std::string>();
  std::string part3 = json["3"].get<std::string>();

  {
    std::ofstream file{"inputs/input1.txt", std::ios::binary};
    std::string input = json["1"].get<std::string>();

    file << input;
  }

  {
    std::ofstream file{"inputs/input2.txt", std::ios::binary};
    std::string input = json["2"].get<std::string>();

    file << input;
  }

  {
    std::ofstream file{"inputs/input3.txt", std::ios::binary};
    std::string input = json["3"].get<std::string>();

    file << input;
  }
}

void ec::Runtime::run() {
  if (do_download_inputs) {
    download_inputs();
  }

  if (interactive == false) {
    run_noninteractive(*this);
  } else {
    run_interactive(*this);
  }
}
