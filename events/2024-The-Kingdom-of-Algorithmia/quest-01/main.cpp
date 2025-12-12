#include <fstream>
#include <iostream>

void part1() {
  std::ifstream input{"input1.txt"};

  int output = 0;

  while (true) {
    char c;
    c = input.get();

    if (input.eof())
      break;

    if (c == 'B')
      output += 1;
    else if (c == 'C')
      output += 3;
  }

  std::cout << output << '\n';
}

void part2() {
  std::ifstream input{"input2.txt"};

  int output = 0;

  while (true) {
    char c1;
    c1 = input.get();
    char c2;
    c2 = input.get();

    if (input.eof())
      break;

    if (c1 == 'x' || c2 == 'x') {
      char c = 0;
      if (c1 == 'x')
        c = c2;
      else
        c = c1;

      if (c == 'B')
        output += 1;
      else if (c == 'C')
        output += 3;
      else if (c == 'D')
        output += 5;
    } else {
      output += 2;
      if (c1 == 'B')
        output += 1;
      else if (c1 == 'C')
        output += 3;
      else if (c1 == 'D')
        output += 5;

      if (c2 == 'B')
        output += 1;
      else if (c2 == 'C')
        output += 3;
      else if (c2 == 'D')
        output += 5;
    }
  }

  std::cout << output << '\n';
}

void part3() {
  std::ifstream input{"input3.txt"};

  int output = 0;

  while (true) {
    int group_strength = 0;
    int all_monsters = 0;

    for (int i = 0; i < 3; ++i) {
      char c;
      c = input.get();

      if (input.eof())
        goto input_ended;

      if (c == 'x')
        continue;
      all_monsters += 1;

      if (c == 'B')
        group_strength += 1;
      else if (c == 'C')
        group_strength += 3;
      else if (c == 'D')
        group_strength += 5;
    }

    if (all_monsters == 2)
      group_strength += 2;
    else if (all_monsters == 3)
      group_strength += 6;

    output += group_strength;
  }

input_ended:
  std::cout << output << '\n';
}

int main() {
  part1();
  part2();
  part3();
}
