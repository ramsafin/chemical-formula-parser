#ifndef CHEMICAL_FORMULA_PARSER_HPP
#define CHEMICAL_FORMULA_PARSER_HPP

/**
 * @file library.hpp
 * @brief Arithmetic operations library (add, subtract, multiply).
 */

namespace cfp {

/**
 * @brief Adds two integers.
 *
 * @param left First integer.
 * @param right Second integer.
 * @return Sum of left and right.
 */
int add(int left, int right);

/**
 * @brief Subtracts two integers.
 *
 * @param left First integer.
 * @param right Second integer.
 * @return Difference of left and right.
 */
int subtract(int left, int right);

/**
 * @brief Multiplies two integers.
 *
 * @param left First integer.
 * @param right Second integer.
 * @return Multiplication of left and right.
 */
int multiply(int left, int right);

/**
 * @brief Divide two integers.
 *
 * @param left First integer.
 * @param right Second integer.
 * @return Division of left and right.
 */
int divide(int left, int right);

}  // namespace cfp

#endif
