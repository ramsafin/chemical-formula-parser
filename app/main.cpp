#include <iostream>

#include "cfp/parser.hpp"
#include "cfp/parser_error.hpp"
#include "cfp/tokenizer_error.hpp"

int main() {
  cfp::Parser parser{"H2O"};

  try {
    for (const auto &[element, count] : parser.parse()) {
      std::cout << element << ": " << count << '\n';
    }
  } catch (const cfp::TokenizerError &e) {
    std::cerr << "Lexing failed:\n" << e.what() << "\n";
    return 2;
  } catch (const cfp::ParserError &e) {
    std::cerr << "Parsing failed:\n" << e.what() << "\n";
    return 3;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error:\n" << e.what() << "\n";
    return 4;
  }

  return 0;
}
