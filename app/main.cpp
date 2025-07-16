#include <exception>
#include <iostream>
#include <string>

#include "cfp/error/parser_error.hpp"
#include "cfp/error/tokenizer_error.hpp"
#include "cfp/parser.hpp"

void process(const std::string &formula) {
  try {
    cfp::Parser parser{formula};
    const auto counts = parser.parse();

    for (const auto &[elem, count] : counts) {
      std::cout << elem << ": " << count << "\n";
    }
  } catch (const cfp::TokenizerError &err) {
    std::cerr << "Lex error: " << err.what() << "\n";
  } catch (const cfp::ParserError &err) {
    std::cerr << "Parse error: " << err.what() << "\n";
  } catch (const std::exception &err) {
    std::cerr << "Error: " << err.what() << "\n";
  }
}

int main(int argc, char **argv) {
  if (argc > 1) {
    // each argument is a formula
    for (int idx = 1; idx < argc; idx++) {
      std::cout << "Formula: " << argv[idx] << "\n";
      process(argv[idx]);
      std::cout << "----\n";
    }
    return 0;
  }

  // no args (interactive mode)
  std::string line;
  std::cout << "Enter formula (Ctrl-D to quit):\n> ";

  while (std::getline(std::cin, line)) {
    if (!line.empty()) {
      process(line);
    }
    std::cout << "> ";
  }

  return 0;
}
