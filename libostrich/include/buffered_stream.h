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

#ifndef __BUFFERED_STREAM_H__
#define __BUFFERED_STREAM_H__

#include <array>
#include <cctype>
#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <ostream> // For std::endl support. libostrich streams don't actually
                   // derive from std::ostream.
#include <vector>

#include "formatting.h"
#include "ostrich.h"

namespace Ostrich {
// Circular queue for one-reader one-writer use.
// Only Full() and Empty() are defined if kSize is 0 (they both return true).
template <std::size_t kSize>
class CircularQueue {
 public:
  CircularQueue() : push_pos_(0), pop_pos_(0) {}

  // Undefined if queue is full.
  void Push(char c) volatile {
    buf_[push_pos_] = c;
    push_pos_ = (push_pos_ + 1) % kSize;
  }

  // Undefined if queue is empty.
  char Pop() volatile {
    char c = buf_[pop_pos_];
    pop_pos_ = (pop_pos_ + 1) % kSize;
    return c;
  }

  char Peek() const volatile {
    return buf_[pop_pos_];
  }

  bool Empty() const volatile {
    if (kSize == 0) {
      return true;
    }
    return push_pos_ == pop_pos_;
  }

  bool Full() const volatile {
    if (kSize == 0) {
      return true;
    }
    return (push_pos_ + 1) % kSize == pop_pos_;
  }

  std::size_t Available() const volatile {
    return (push_pos_ >= pop_pos_) ?
        (push_pos_ - pop_pos_) : (push_pos_ + kSize - pop_pos_);
  }

  std::size_t Capacity() const volatile {
    return kSize;
  }

 private:
  char buf_[kSize];
  std::size_t push_pos_;
  std::size_t pop_pos_;
};

// The Buffered*Stream class provides an interface similar to std::iostream, but
// with only basic formating functionality and is much lighter weight.
// Input is always buffered.
template <std::size_t kInputBufferSize>
class BufferedInputStream {
 public:
  BufferedInputStream() : delim_func_([](char c){ return std::isspace(c); }) {}

  // Input operators.
  // Reading into char buffer is unsafe and intentionally omitted.
  BufferedInputStream& operator>>(std::string& s) {
    s = ReadUntilDelim();
    return *this;
  }

  // Bool
  template <typename T,
            typename std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
  BufferedInputStream& operator>>(T& x) {
    // Read bools as int. This is the same as std::istream behaviour when
    // boolalpha is off.
    if (Parse<int>(ReadUntilDelim())) {
      x = true;
    } else {
      x = false;
    }
    return *this;
  }

  // 8-bit types
  template <typename T,
            typename std::enable_if_t<
              std::is_integral<T>::value &&
              (std::numeric_limits<T>::digits == 8 ||
               std::numeric_limits<T>::digits == 7),
            int> = 0>
  BufferedInputStream& operator>>(T& c) {
    BlockUntilInputAvailable();
    c = GetNextChar();
    return *this;
  }

  // All other integral and floating point types.
  template <typename T,
            typename std::enable_if_t<
              std::is_integral<T>::value &&
              (std::numeric_limits<T>::digits > 8) ||
              std::is_floating_point<T>::value, 
            int> = 0>
  BufferedInputStream& operator>>(T& x) {
    x = Parse<T>(ReadUntilDelim());
    return *this;
  }

  std::string GetLine() {
    std::string ret;
    char c;
    do {
      BlockUntilInputAvailable();
      c = GetNextChar();
      if (c == '\n') {
        break;
      } else {
        ret.push_back(c);
      }
    } while (true);
    return ret;
  }

  void Read(char* buf, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      BlockUntilInputAvailable();
      buf[i] = GetNextChar();
    }
  }

  std::size_t DataAvailable() const {
    return input_buffer_.Available();
  }

  std::size_t ReceiveBufferSpace() const {
    return input_buffer_.Capacity() - input_buffer_.Available();
  }

 protected:
  // Derived classes can use this function as a signal to provide data using the
  // AddDataToBuffer() function. If AddDataToBuffer() is called from an ISR for
  // example, calls to this function may be ignored.
  virtual void InputData(std::size_t /*requested_data_len*/) { return; }

  // Derived classes can use this as a signal that data has been read by the
  // user, so that if the input buffer was full before, it may no longer be.
  virtual void InputDataRead() { return; }

  // Derived classes use this function to add data to the input buffer. The
  // derived class can either call this function as a response to RequestData(),
  // or from an ISR. This is the only function that may be called from interrupt
  // context.
  // This function should not be overridden, only called by the derived class.
  void AddDataToBuffer(
      const char* data, std::size_t len) volatile {
    for (std::size_t i = 0; i < len; ++i) {
      if (input_buffer_.Full()) {
        // Drop data if buffer is already full.
        break;
      } else {
        input_buffer_.Push(data[i]);
      }
    }
  }

 private:
  // Gobble up everything that is delim according to delim_func_, then return
  // a string with all characters until the next char that is delim (the delim
  // is left in the stream).
  std::string ReadUntilDelim() {
    char c;
    do {
      BlockUntilInputAvailable();
      c = input_buffer_.Peek();
      if (delim_func_(c)) {
        GetNextChar();
      } else {
        break;
      }
    } while (true);

    std::string ret;
    do {
      BlockUntilInputAvailable();
      c = input_buffer_.Peek();
      if (!delim_func_(c)) {
        ret.push_back(c);
        GetNextChar();
      } else {
        break;
      }
    } while (true);

    return ret;
  }

  void BlockUntilInputAvailable() {
    while (input_buffer_.Empty()) {
      WaitForInterrupt();
    }
  }

  char GetNextChar() {
    char c = input_buffer_.Pop();
    InputDataRead();
    return c;
  }

  volatile CircularQueue<kInputBufferSize> input_buffer_;
  std::function<bool(char)> delim_func_;
};

// Output buffering can be disabled by setting kOutputBufferSize to 0. This is
// slightly faster for streams that output data one byte at a time anyways, and
// don't benefit from larger block writes.
template <std::size_t kOutputBufferSize>
class BufferedOutputStream {
 public:
  void Flush() { FlushOutput(); }

  // Output operators.
  BufferedOutputStream& operator<<(const std::string& s) {
    EnqueueOutput(s.data(), s.size());
    return *this;
  }

  BufferedOutputStream& operator<<(const char* s) {
    EnqueueOutput(s, strlen(s));
    return *this;
  }

  // bool
  template <typename T,
            typename std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
  BufferedOutputStream& operator<<(T c) {
    if (c) {
      EnqueueOutput("1");
    } else {
      EnqueueOutput("0");
    }
    return *this;
  }

  // 8-bit types
  template <typename T,
            typename std::enable_if_t<
              std::is_integral<T>::value &&
              (std::numeric_limits<T>::digits == 8 ||
               std::numeric_limits<T>::digits == 7),
            int> = 0>
  BufferedOutputStream& operator<<(T c) {
    EnqueueOutput(&c, 1);
    return *this;
  }

  // All other integral and floating point types.
  template <typename T,
            typename std::enable_if_t<
              std::is_integral<T>::value &&
              (std::numeric_limits<T>::digits > 8) ||
              std::is_floating_point<T>::value, 
            int> = 0>
  BufferedOutputStream& operator<<(T x) {
    EnqueueOutput(Format(x));
    return *this;
  }

  // We use this to catch std::endl and std::flush. All other manipulators are
  // no-op.
  BufferedOutputStream& operator<<(
    std::ostream& (*manipulator)(std::ostream&)) {
    using char_type = std::ostream::char_type;
    using traits_type = std::ostream::traits_type;
    if (manipulator == &std::endl<char_type, traits_type>) {
      EnqueueOutput("\n", 1);
      Flush();
    } else if (manipulator == &std::flush<char_type, traits_type>) {
      Flush();
    }
    return *this;
  }

  void Write(char* buf, std::size_t size) {
    EnqueueOutput(buf, size);
  }

 protected:
  // Derived classes use this function to implement output to the underlying
  // stream.
  virtual void OutputImpl(const char* data, std::size_t len) = 0;

  // Derived classes can optionally override this function to specify their
  // minimum optimal write size. BufferedStream will try to buffer for this
  // length before callign OutputImpl().
  virtual std::size_t OptimalWriteBlockSize() const { return 1; }

 private:
  void FlushOutput() {
    if (kOutputBufferSize == 0 || output_buffer_.Empty()) { return; }

    // Flush the data to OutputImpl(). We have to make a copy for now.
    // We will optimize this if it becomes a bottleneck.
    // The number of available bytes may increase while we are copying. This is
    // ok. We will just copy the number of bytes already there when we get here.
    std::vector<char> data_copy(output_buffer_.Available());
    for (std::size_t i = 0; i < data_copy.size(); ++i) {
      data_copy[i] = output_buffer_.Pop();
    }
    OutputImpl(data_copy.data(), data_copy.size());
  }

  void EnqueueOutput(const char* data, std::size_t len) {
    if (kOutputBufferSize == 0) {
      OutputImpl(data, len);
    } else {
      for (std::size_t i = 0; i < len; ++i) {
        if (output_buffer_.Full()) {
          FlushOutput();
        }
        output_buffer_.Push(data[i]);
      }

      if (output_buffer_.Available() >= OptimalWriteBlockSize()) {
        FlushOutput();
      }
    }
  }

  void EnqueueOutput(const std::string& s) {
    EnqueueOutput(s.data(), s.size());
  }

  CircularQueue<kOutputBufferSize> output_buffer_;
};

using UnbufferedOutputStream = BufferedOutputStream<0>;

} // namespace Ostrich

#endif // __BUFFERED_STREAM_H__