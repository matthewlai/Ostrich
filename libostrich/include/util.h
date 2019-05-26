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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <utility>

#include "ostrich.h"

namespace Ostrich {

inline void WaitForInterrupt() {
  __asm__("wfi");
}

class Singleton {
 protected:
	Singleton() {}
	~Singleton() {} // Do not allow deleting through base pointer
 private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};

class NonCopyable {
 public:
  NonCopyable() {}

  NonCopyable(NonCopyable&&) {}
  NonCopyable& operator=(NonCopyable&&) { return *this; }

 private:
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

// This is a much simplified version of the C++ std::future/std::promise system.
// We only have a future class, and solve the shared ownership problem by
// forcing the consumer to hold on to the future until the value is available.
// Destructor blocks until value is received.
// Since we block with the "wfi" instruction, the future must be fulfilled
// through interrupt.
// Each future should be set exactly once, and read at most once.
template <typename T>
class Future : public NonCopyable {
 public:
  Future() : value_received_(false) {}

  void SetValue(const T& value) {
    value = value;
    value_received_ = true;
  }

  void SetValue(T&& value) {
    value_ = std::move(value);
    value_received_ = true;
  }

  T&& GetValue() {
    while (!value_received_) {
      WaitForInterrupt();
    }

    return std::move(value_);
  }

  ~Future() {
    // Wait for value to arrive, because otherwise the the producer writes to
    // this future we may no longer exist.
    while (!value_received_) {
      WaitForInterrupt();
    }
  }

 private:
  volatile T value_;
  volatile bool value_received_;
};

}; // namespace Ostrich

#endif // __UTIL_H__
