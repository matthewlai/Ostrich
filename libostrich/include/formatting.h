/*
 * This file is part of the libostrich project.
 *
 * Copyright (C) 2019 Matthew Lai <m@matthewlai.ca>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FORMATTING_H__
#define __FORMATTING_H__

#include <cctype>
#include <cfloat>
#include <cmath>
#include <type_traits>

namespace Ostrich {

static const char kDigits[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

// Integral types.
template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline std::string Format(T x, int base = 10) {
  std::string ret;
  if (std::is_signed<T>() && x < 0) {
    ret += "-";

    // This is safe even if x is the most negative value that can be
    // represented, because -x would result in the same value, and then we are
    // casting it to unsigned, which will have the correct value.
    x *= -1;
  }

  if (x == 0) {
    return "0";
  }

  using U = typename std::make_unsigned<T>::type;
  U x_u = static_cast<U>(x);
  std::string val;
  while (x_u) {
    U quotient = x_u / base;
    U remainder = x_u % base;
    val.push_back(kDigits[remainder]);
    x_u = quotient;
  }

  ret += std::string(val.rbegin(), val.rend());

  return ret;
}

// Some simple standard library replacement functions for size (these functions
// don't support special values, and only work with positive values).
template <typename T, typename E,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline T Pow(T x, E e) {
  static_assert(std::is_integral<E>::value,
                "Only integral powers are supported");
  if (e == 0) {
    return T(1);
  } else if (e == 1) {
    return x;
  } else if (e < 0) {
    return T(1) / Pow(x, -e);
  } else {
    T part = Pow(x, e / 2);
    if (e % 2 == 0) {
      return part * part;
    } else {
      return part * part * x;
    }
  } 
}

template <typename T,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline T Round(T x) {
  T int_part;
  T frac_part;
  frac_part = std::modf(x, &int_part);
  return int_part + (frac_part >= static_cast<T>(0.5) ?
      static_cast<T>(1.0) : 0.0);
}

// Convert an integer stored in a FP type. This is used by Format() below.
template <typename T,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline std::string ConvertFPInt(T x, int base) {
  std::string ret;

  // We don't want to cast to an integer type because
  // we want to be able to print very big floating point numbers. But the
  // algorithm is similar to the one in Format() for integer types.
  int exp = 0;

  // Find the largest exp where base^exp is smaller than x.
  while (Pow(static_cast<T>(base), exp + 1) <= x) {
    ++exp;
  }

  while (exp >= 0) {
    T divisor = Pow(static_cast<T>(base), exp);
    int digit = static_cast<int>(x / divisor);
    ret.push_back(kDigits[digit]);
    x = std::fmod(x, divisor);
    --exp;
  }

  return ret;
}

// Floating point types.
// If fabs(x) < kScientificNotationThreshold, we use scientific notation.
constexpr double kScientificNotationThreshold = 0.1;

// All floating point parsing/formatting is in base 10.
constexpr int kFPBase = 10;
template <typename T,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline std::string Format(T x, int precision = 3, int base = 10) {
  int classification = std::fpclassify(x);

  if (classification == FP_INFINITE) {
    return x > 0.0 ? "inf" : "-inf";
  }

  if (classification == FP_NAN) {
    return "nan";
  }

  int sci_notation_digits = 0;
  if (fabs(x) < kScientificNotationThreshold) {
    sci_notation_digits = std::floor(std::log10(x));
    x *= Pow(static_cast<T>(kFPBase), -sci_notation_digits);
  }

  // We are left with normal, subnormal, and zeros. Our implementation here can
  // deal with all of them.
  std::string ret;
  if (std::signbit(x)) {
    ret.push_back('-');

    x = std::copysign(x, static_cast<T>(1.0));
  }

  T int_part;
  T frac_part;
  frac_part = std::modf(x, &int_part);
  
  ret.append(ConvertFPInt(int_part, base));

  if (precision > 0) {
    ret.push_back('.');
    frac_part *= Pow(static_cast<T>(base), precision);
    std::string frac_part_str = ConvertFPInt(Round(frac_part), base);
    if (frac_part_str.size() < static_cast<std::size_t>(precision)) {
      ret.append(precision - frac_part_str.size(), '0');
    }
    ret.append(frac_part_str);
  }

  if (sci_notation_digits != 0) {
    ret.append("e");
    ret.append(Format(sci_notation_digits));
  }

  return ret;
}

template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline T Parse(const std::string& s) {
  T ret = 0;
  bool neg = false;

  // We only allow changing base until we see a non-zero.
  bool seen_nonzero = false;

  // Base is 10 until we discover otherwise (0x..., 0b..., 0...)
  int base = 10;

  for (const char& c : s) {
    if (c == '-') {
      neg = true;
    } else if (c == 'x' && !seen_nonzero) {
      ret = 0;
      base = 16;
    } else if (c == '0' && !seen_nonzero) {
      ret = 0;
      base = 8;
    } else {
      seen_nonzero = true;
      int digit = 0;
      for (int i = 0; i < base; ++i) {
        if (std::tolower(c) == kDigits[i]) {
          digit = i;
          break;
        }
      }
      ret *= base;
      ret += digit;
    }
  }

  if (std::is_signed<T>() && neg) {
    ret *= -1;
  }

  return ret;
}

// Floating point numbers are always parsed in base 10.
// This is a helper function for parsing an integer into a FP type. The integer
// must be positive.
template <typename T,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline T ParseFPInt(const std::string& s) {
  T ret = 0;

  for (const char& c : s) {
    int digit = 0;
    for (int i = 0; i < kFPBase; ++i) {
      if (c == kDigits[i]) {
        digit = i;
        break;
      }
    }
    ret *= kFPBase;
    ret += digit;
  }

  return ret;
}

template <typename T,
          typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline T Parse(const std::string& s) {
  T ret = 0;
  bool neg = false;
  bool exp_neg = false;

  // We first split the number into 3 optional parts - before decimal, after
  // decimal, and exp. Sign bit is stored separately from all 3.
  std::string before_decimal;
  std::string after_decimal;
  std::string exp;

  enum class ParseStage {
    kBeforeDecimal, kAfterDecimal, kExp
  };

  ParseStage stage = ParseStage::kBeforeDecimal;

  for (const char& c : s) {
    switch (stage) {
      case ParseStage::kBeforeDecimal:
        if (c == '.') {
          stage = ParseStage::kAfterDecimal;
        } else if (c == 'e' || c == 'E') {
          stage = ParseStage::kExp;
        } else if (c == '-') {
          neg = true;
        } else {
          before_decimal.push_back(c);
        }
        break;
      case ParseStage::kAfterDecimal:
        if (c == 'e' || c == 'E') {
          stage = ParseStage::kExp;
        } else {
          after_decimal.push_back(c);
        }
        break;
      case ParseStage::kExp:
        if (c == '-') {
          exp_neg = true;
        } else {
          exp.push_back(c);
        }
        break;
    }
  }

  T int_part = ParseFPInt<T>(before_decimal);

  T frac_part = ParseFPInt<T>(after_decimal);
  frac_part /= Pow(static_cast<T>(kFPBase), after_decimal.size());

  T exp_part = ParseFPInt<T>(exp);
  exp_part = Pow(static_cast<T>(kFPBase), static_cast<int>(exp_part));

  ret = (int_part + frac_part) * (exp_neg ? (1.0f / exp_part) : exp_part);

  if (std::is_signed<T>() && neg) {
    ret *= -1;
  }

  return ret;
}

} // namespace Ostrich

#endif // __FORMATTING_H__