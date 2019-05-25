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

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "buffered_stream.h"
#include "gpio.h"
#include "util.h"

namespace Ostrich {

class USBSerial : public BufferedInputStream<512>,
                  public BufferedOutputStream<64> {
 public:
  // The default PIDs here are testing PIDs (http://pid.codes/1209/0001/).
  // Make sure to change them before redistributing or selling any device!
  USBSerial(uint16_t vid = 0x1209, uint16_t pid = 0x0001,
            uint16_t current_ma = 100, const char* manufacturer = "Ostrich",
            const char* product = "CDC-ACM");
  ~USBSerial();

  USBSerial(const USBSerial&) = delete;
  USBSerial& operator=(const USBSerial&) = delete;

  // An application on the host has opened the serial port.
  bool PortOpen() { return dtr_; }

  void Poll() { usbd_poll(usbd_dev_); }

  uint32_t BaudRate() const { return baud_rate_; }

 protected:
  void OutputImpl(const char* data, std::size_t len) override;

  // 64 bytes is the maximum packet length in FullSpeed mode.
  std::size_t OptimalWriteBlockSize() const override { return 64; }

  void InputDataRead() override {
    if (endpoint_nak_) {
      if (ReceiveBufferSpace() >= 64) {
        endpoint_nak_ = false;
        usbd_ep_nak_set(usbd_dev_, 0x01, 0);
      }
    }
  }

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

  GPIOManager::PinAllocation pin_allocation_dm_;
  GPIOManager::PinAllocation pin_allocation_dp_;

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

  // Whether we have told the endpoint to stop accepting data (because buffer is
  // full).
  volatile bool endpoint_nak_;

  volatile uint32_t baud_rate_;
};

} // namespace Ostrich

#endif // __USB_SERIAL_H__
