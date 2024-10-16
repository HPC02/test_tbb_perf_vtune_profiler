#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>  // std::cerr

#ifndef CHECK1

#define CHECK1(condition, message)                                                                                        \
  (!(condition)) ? (std::cerr << "Assertion failed: (" << #condition << "), " << "function " << __FUNCTION__ << ", file " \
                              << __FILE__ << ", line " << __LINE__ << "." << std::endl                                    \
                              << message << std::endl,                                                                    \
                    abort(), 0)                                                                                           \
                 : 1

#endif  // CHECK1

#ifndef CHECK2
#if defined DEBUG || defined _DEBUG
#define CHECK2(condition, message)                                                                                        \
  (!(condition)) ? (std::cerr << "Assertion failed: (" << #condition << "), " << "function " << __FUNCTION__ << ", file " \
                              << __FILE__ << ", line " << __LINE__ << "." << std::endl                                    \
                              << message << std::endl,                                                                    \
                    abort(), 0)                                                                                           \
                 : 1
#else
#define CHECK2(condition, message) (void)0
#endif
#endif  // CHECK2

#include <algorithm>  // for std::max

// https://bitbashing.io/comparing-floats.html
// https://www.learncpp.com/cpp-tutorial/relational-operators-and-floating-point-comparisons/

template <typename T>
constexpr T constAbs(T x) {
  return (x < 0 ? -x : x);
}

// Return true if the difference between a and b is within epsilon percent of
// the larger of a and b
constexpr bool approximatelyEqualRel(double a, double b, double relEpsilon) {
  return (constAbs(a - b) <= (std::max(constAbs(a), constAbs(b)) * relEpsilon));
}

// Return true if the difference between a and b is less than or equal to
// absEpsilon, or within relEpsilon percent of the larger of a and b
constexpr bool approximatelyEqualAbsRel(double a, double b, double absEpsilon, double relEpsilon) {
  // Check if the numbers are really close -- needed when comparing numbers near zero.
  if (constAbs(a - b) <= absEpsilon) return true;

  // Otherwise fall back to Knuth's algorithm
  return approximatelyEqualRel(a, b, relEpsilon);
}
