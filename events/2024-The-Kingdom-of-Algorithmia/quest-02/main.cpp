#include <fstream>
#include <iostream>
#include <set>

#include <everybody-codes.h>
#include <string>

std::vector<std::string> get_symbols(const std::string &line) {
  int prev_pos = 0;
  std::vector<std::string> runic_words;

  while (true) {
    int comma = line.find(',', prev_pos);

    auto runic_word = line.substr(prev_pos, comma - prev_pos);
    runic_words.emplace_back(std::move(runic_word));

    if (comma == std::string::npos)
      break;

    prev_pos = comma + 1;
  }

  return runic_words;
}

std::string part1(const std::string &input_filename) {
  std::ifstream input{input_filename};

  std::string first_line;
  std::getline(input, first_line);

  first_line = first_line.substr(6);
  auto runic_words = get_symbols(first_line);

  std::getline(input, first_line);

  std::string text;
  std::getline(input, text);

  long long all_words_occur = 0;

  for (auto &word : runic_words) {
    int prev_found = 0;
    while (true) {
      int found = text.find(word, prev_found);

      if (found == std::string::npos)
        break;

      prev_found = found + 1;
      all_words_occur += 1;
    }
  }

  return std::to_string(all_words_occur);
}

std::string part2(const std::string &input_filename) {
  std::ifstream input{input_filename};

  std::string first_line;
  std::getline(input, first_line);

  first_line = first_line.substr(6);
  auto runic_words = get_symbols(first_line);

  std::getline(input, first_line);

  long long all_words_occur = 0;

  while (!input.eof()) {
    std::string line;
    std::getline(input, line);

    std::set<int> set;
    for (auto &word : runic_words) {
      int prev_found = 0;
      while (true) {
        int found = line.find(word, prev_found);

        if (found == std::string::npos)
          break;

        prev_found = found + 1;
        // all_words_occur += word.length();
        for (int i = 0; i < word.length(); ++i) {
          set.insert(found + i);
        }
      }
    }

    std::string reversed{line.rbegin(), line.rend()};
    for (auto &word : runic_words) {
      int prev_found = 0;
      while (true) {
        int found = reversed.find(word, prev_found);

        if (found == std::string::npos)
          break;

        prev_found = found + 1;
        // all_words_occur += word.length();
        for (int i = 0; i < word.length(); ++i) {
          set.insert(line.length() - found - i - 1);
        }
      }
    }
    all_words_occur += set.size();
  }

  return std::to_string(all_words_occur);
}

std::string part3(const std::string &input_filename) {
  std::ifstream input{input_filename};

  std::string first_line;
  std::getline(input, first_line);

  first_line = first_line.substr(6);
  auto runic_words = get_symbols(first_line);

  std::getline(input, first_line);

  long long all_words_occur = 0;

  std::vector<std::vector<char>> map;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);

    if (input.eof())
      break;

    std::vector<char> row;

    for (char c : line) {
      row.push_back(c);
    }

    map.emplace_back(std::move(row));
  }

  std::set<std::pair<int, int>> set;
  for (int y = 0; y < map.size(); ++y) {
    for (int x = 0; x < map.at(0).size(); ++x) {
      for (auto &word : runic_words) {
        // LTR
        {
          bool good = true;
          for (int i = 0; i < word.length(); ++i) {
            if (map.at(y).at((x + i) % map.at(0).size()) != word.at(i)) {
              good = false;
              break;
            }
          }

          if (good) {
            for (int i = 0; i < word.length(); ++i) {
              set.insert({y, (x + i) % map.at(0).size()});
            }
          }
        }

        // RTL
        {
          bool good = true;
          int width = map.at(0).size();
          for (int i = 0; i < word.length(); ++i) {
            if (map.at(y).at(((x - i) % width + width) % width) != word.at(i)) {
              good = false;
              break;
            }
          }

          if (good) {
            for (int i = 0; i < word.length(); ++i) {
              set.insert({y, ((x - i) % width + width) % width});
            }
          }
        }

        // TTB
        {
          bool good = true;
          int height = map.size();
          if (height - y >= word.length()) {
            for (int i = 0; i < word.length(); ++i) {
              if (map.at(y + i).at(x) != word.at(i)) {
                good = false;
                break;
              }
            }

            if (good) {
              for (int i = 0; i < word.length(); ++i) {
                set.insert({y + i, x});
              }
            }
          }
        }

        // BTT
        {
          bool good = true;
          int height = map.size();
          if (y - (int)word.length() + 1 >= 0) {
            for (int i = 0; i < word.length(); ++i) {
              if (map.at(y - i).at(x) != word.at(i)) {
                good = false;
                break;
              }
            }

            if (good) {
              for (int i = 0; i < word.length(); ++i) {
                set.insert({y - i, x});
              }
            }
          }
        }
      }
    }
  }

  return std::to_string(set.size());
}

int main(int argc, char *argv[]) {
  ec::Runtime rt{argc, argv};

  rt.part1.input.filename = "inputs/input1.txt";
  rt.part1.tests.push_back(
      {.filename = "inputs/example1.txt", .expected_result = "4"});
  rt.part1.impl = part1;

  rt.part2.input.filename = "inputs/input2.txt";
  rt.part2.tests.push_back(
      {.filename = "inputs/example2.txt", .expected_result = "42"});
  rt.part2.impl = part2;

  rt.part3.input.filename = "inputs/input3.txt";
  rt.part3.tests.push_back(
      {.filename = "inputs/example3.txt", .expected_result = "10"});
  rt.part3.impl = part3;

  rt.run();
}
