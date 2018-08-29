/*
 * This file is part of the libostrich project.
 *
 * Copyright (C) 2018 Matthew Lai <m@matthewlai.ca>
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

#ifndef __USB_SERIAL_H__
#define __USB_SERIAL_H__

#include <algorithm>
#include <cstddef>
#include <istream>
#include <streambuf>
#include <string>
#include <queue>

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "gpio.h"

namespace Ostrich {

// Circular queue for one-reader one-writer use.
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
    return push_pos_ == pop_pos_;
  }

  bool Full() const volatile {
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

// We have a strange setup here. USBSerial derives from both streambuf and iostream.
// It is primarily a streambuf, and the iostream inheritance allows the user to use
// it as an iostream wrapped around itself. We call the iostream constructor with
// "this" in our constructor, and that's the only thing we do with iostream here.
class USBSerial : public std::streambuf, public std::iostream {
 public:
  USBSerial();
  ~USBSerial();

  USBSerial(const USBSerial&) = delete;
  USBSerial& operator=(const USBSerial&) = delete;

  // These typedefs exist in both streambuf and iostream, so we need to disambiguate.
  using std::streambuf::int_type;
  using std::streambuf::traits_type;
  using std::streambuf::char_type;

  // An application on the host has opened the serial port.
  bool PortOpen() { return dtr_; }

  void Send(const char* data, std::size_t length);
  void Send(const std::string& s) {
    Send(s.data(), s.size());
  }

  std::size_t DataAvailable() const {
    return receive_buffer_.Available();
  }

  // Non-blocking receive.
  std::size_t Receive(char* buf, std::size_t buf_size) {
    std::size_t to_read = std::min(buf_size, receive_buffer_.Available());
    for (std::size_t i = 0; i < to_read; ++i) {
      buf[i] = receive_buffer_.Pop();
    }
    return to_read;
  }

  // Wait for the receive buffer to become non-empty.
  void WaitForData() {
    while (receive_buffer_.Empty()) {}
  }

  void Poll() { usbd_poll(usbd_dev_); }

 protected:
  // std::streambuf interface (outputs)
  std::streamsize xsputn(const char_type* s, std::streamsize count) override;
  int_type overflow(int_type ch = traits_type::eof()) override;

  // std::streambuf interface (inputs)
  std::streamsize showmanyc() override;
  int_type underflow() override;
  int_type uflow() override;

 private:
 	struct CDCFunctionalDescriptors {
 		usb_cdc_header_descriptor header;
		usb_cdc_call_management_descriptor call_mgmt;
		usb_cdc_acm_descriptor acm;
		usb_cdc_union_descriptor cdc_union;
 	} __attribute__((packed));

  static void SetConfigCallback(usbd_device* usbd_dev, uint16_t wValue);
  static void DataRxCallback(usbd_device* usbd_dev, uint8_t endpoint);
  static usbd_request_return_codes ControlRequestCallback(
  	  usbd_device* usbd_dev, usb_setup_data* req, uint8_t** buf, uint16_t* len,
	  void (**complete)(usbd_device* usbd_dev, usb_setup_data* req));

  usb_device_descriptor GetDeviceDescriptor(uint16_t vid, uint16_t pid);
  usb_config_descriptor GetConfigDescriptor(uint32_t max_current_ma);
  usb_interface_descriptor GetCommInterface();
  usb_interface_descriptor GetDataInterface();
  CDCFunctionalDescriptors GetCDCFunctionalDescriptors();

  usbd_device* usbd_dev_;
  usb_device_descriptor dev_descriptor_;
  usb_config_descriptor config_descriptor_;
  CDCFunctionalDescriptors cdc_functional_descriptors_;
  usb_interface interfaces_[2];
  usb_interface_descriptor comm_interface_;
  usb_interface_descriptor data_interface_;
  usb_endpoint_descriptor comm_endpoints_[1];
  usb_endpoint_descriptor data_endpoints_[2];
  const char* usb_strings_[3];
  uint8_t control_buffer_[256];
  char unique_id_[13];

  volatile bool dtr_;

  volatile CircularQueue<1024> receive_buffer_;
};

} // namespace Ostrich

#endif // __USB_SERIAL_H__
