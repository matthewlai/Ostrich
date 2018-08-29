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

#include "usb/serial.h"

#include <algorithm>
#include <cstring>
#include <type_traits>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/cdc.h>

#include "ostrich.h"

namespace {

// There should only ever be one USBSerial. We go into spin loop for 
// debugging if user tries to construct more than one.
Ostrich::USBSerial* g_usb_serial = nullptr;

template <typename T>
void ZeroInit(T* obj) {
	static_assert(std::is_pod<T>::value, "Can only zero init POD types");
	std::memset(obj, 0, sizeof(T));
}

} // namespace

extern "C" {
void otg_fs_isr() {
	if (g_usb_serial) {
		g_usb_serial->Poll();
	}
}
}

namespace Ostrich {

USBSerial::USBSerial(uint16_t vid, uint16_t pid, uint16_t current_ma,
										 const char* manufacturer, const char* product)
		: std::iostream(this),
			usbd_dev_(nullptr),
			dev_descriptor_(GetDeviceDescriptor(vid, pid)),
			config_descriptor_(GetConfigDescriptor(current_ma)),
			cdc_functional_descriptors_(GetCDCFunctionalDescriptors()),
			comm_interface_(GetCommInterface()),
			data_interface_(GetDataInterface()),
			dtr_(false) {

	if (g_usb_serial != nullptr) {
		// Only 1 USB serial interface can be constructed!
		LockUp();
	}

  GPIOManager::GetInstance().AllocateAFPin(GpioDef::PIN_A11, GPIO_AF10);
  GPIOManager::GetInstance().AllocateAFPin(GpioDef::PIN_A12, GPIO_AF10);

  desig_get_unique_id_as_dfu(unique_id_);

  usb_strings_[0] = manufacturer;
  usb_strings_[1] = product;
  usb_strings_[2] = unique_id_;

  usbd_dev_ = usbd_init(&otgfs_usb_driver, &dev_descriptor_, &config_descriptor_,
  											usb_strings_, 3, control_buffer_, sizeof(control_buffer_));

  usbd_register_set_config_callback(usbd_dev_, SetConfigCallback);

  g_usb_serial = this;

  nvic_enable_irq(NVIC_OTG_FS_IRQ);
}

USBSerial::~USBSerial() {
	nvic_disable_irq(NVIC_OTG_FS_IRQ);
  rcc_periph_clock_disable(RCC_OTGFS);
  g_usb_serial = nullptr;
}

void USBSerial::Send(const char* data, std::size_t length) {
	ScopedIRQLock irq_lock(NVIC_OTG_FS_IRQ);
	std::size_t length_written = 0;
	while (length_written < length) {
		std::size_t packet_size = std::min((length - length_written), 64u);
		length_written += usbd_ep_write_packet(usbd_dev_, 0x82, data + length_written, packet_size);
	}
}

std::streamsize USBSerial::xsputn(const USBSerial::char_type* s, std::streamsize count) {
	Send(s, count);
	return count;
}

USBSerial::int_type USBSerial::overflow(USBSerial::int_type ch) {
	if (!traits_type::eq_int_type(ch, traits_type::eof())) {
		char_type ch_c = ch;
		Send(&ch_c, 1);
	}
	return 1;
}

std::streamsize USBSerial::showmanyc() {
	return receive_buffer_.Available();
}

USBSerial::int_type USBSerial::underflow() {
	WaitForData();
	return traits_type::to_int_type(receive_buffer_.Peek());
}

USBSerial::int_type USBSerial::uflow() {
	WaitForData();
	return traits_type::to_int_type(receive_buffer_.Pop());
}

/*static*/ void USBSerial::SetConfigCallback(usbd_device* usbd_dev, uint16_t /*wValue*/) {
  usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, DataRxCallback);
  usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, nullptr);
  usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, nullptr);
  usbd_register_control_callback(usbd_dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
  								 							 USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
  								 							 ControlRequestCallback);
}

/*static*/ void USBSerial::DataRxCallback(usbd_device* usbd_dev, uint8_t /*endpoint*/) {
	char buf[64];
	std::size_t len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

	for (std::size_t i = 0; i < len; ++i) {
		if (g_usb_serial->receive_buffer_.Full()) {
			// Drop data if receive buffer is full.
			break;
		}
		g_usb_serial->receive_buffer_.Push(buf[i]);
	}
}

/*static*/ usbd_request_return_codes USBSerial::ControlRequestCallback(
  	usbd_device* /*usbd_dev*/, usb_setup_data* req, uint8_t** /*buf*/, uint16_t* len,
	void (**/*complete*/)(usbd_device* usbd_dev, usb_setup_data* req)) {
  switch (req->bRequest) {
  	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
  		g_usb_serial->dtr_ = req->wValue & 1;
  	  return USBD_REQ_HANDLED;
  	}
  	case USB_CDC_REQ_SET_LINE_CODING: {
  	  if (*len < sizeof(usb_cdc_line_coding)) {
  	  	return USBD_REQ_NOTSUPP;
  	  }

  	  return USBD_REQ_HANDLED;
  	}
  }

  return USBD_REQ_NOTSUPP;
}

usb_device_descriptor USBSerial::GetDeviceDescriptor(uint16_t vid, uint16_t pid) {
  usb_device_descriptor dev;
  ZeroInit(&dev);
  dev.bLength = USB_DT_DEVICE_SIZE;
  dev.bDescriptorType = USB_DT_DEVICE;
  dev.bcdUSB = 0x0100;
  dev.bDeviceClass = USB_CLASS_CDC;
  dev.bDeviceSubClass = 0;
  dev.bDeviceProtocol = 0;
	dev.bMaxPacketSize0 = 64;
	dev.idVendor = vid;
	dev.idProduct = pid;
	dev.bcdDevice = 0x0200;
	dev.iManufacturer = 1;
	dev.iProduct = 2;
  dev.iSerialNumber = 3;
  dev.bNumConfigurations = 1;
  return dev;
}

usb_config_descriptor USBSerial::GetConfigDescriptor(uint32_t max_current_ma) {
	usb_config_descriptor config;
	ZeroInit(&config);
	config.bLength = USB_DT_CONFIGURATION_SIZE;
	config.bDescriptorType = USB_DT_CONFIGURATION;
	config.wTotalLength = 0;
	config.bNumInterfaces = 2;
	config.bConfigurationValue = 1;
	config.iConfiguration = 0;
	config.bmAttributes = 0x80;
	config.bMaxPower = max_current_ma / 2;

	ZeroInit(&interfaces_[0]);
	ZeroInit(&interfaces_[1]);
	interfaces_[0].num_altsetting = 1;
	interfaces_[0].altsetting = &comm_interface_;
	interfaces_[1].num_altsetting = 1;
	interfaces_[1].altsetting = &data_interface_;

	config.interface = interfaces_;
	return config;
}

usb_interface_descriptor USBSerial::GetCommInterface() {
	usb_interface_descriptor iface;
	ZeroInit(&iface);
	iface.bLength = USB_DT_INTERFACE_SIZE;
	iface.bDescriptorType = USB_DT_INTERFACE;
	iface.bInterfaceNumber = 0;
	iface.bAlternateSetting = 0;
	iface.bNumEndpoints = 1;
	iface.bInterfaceClass = USB_CLASS_CDC;
	iface.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	iface.bInterfaceProtocol = USB_CDC_PROTOCOL_AT;
	iface.iInterface = 0;

	ZeroInit(&comm_endpoints_[0]);
	comm_endpoints_[0].bLength = USB_DT_ENDPOINT_SIZE;
	comm_endpoints_[0].bDescriptorType = USB_DT_ENDPOINT;
	comm_endpoints_[0].bEndpointAddress = 0x83;
	comm_endpoints_[0].bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT;
	comm_endpoints_[0].wMaxPacketSize = 16;
	comm_endpoints_[0].bInterval = 255;
	iface.endpoint = comm_endpoints_;

	iface.extra = &cdc_functional_descriptors_;
	iface.extralen = sizeof(CDCFunctionalDescriptors);
	return iface;
}

usb_interface_descriptor USBSerial::GetDataInterface() {
	usb_interface_descriptor iface;
	ZeroInit(&iface);
	iface.bLength = USB_DT_INTERFACE_SIZE;
	iface.bDescriptorType = USB_DT_INTERFACE;
	iface.bInterfaceNumber = 1;
	iface.bAlternateSetting = 0;
	iface.bNumEndpoints = 2;
	iface.bInterfaceClass = USB_CLASS_DATA;
	iface.bInterfaceSubClass = 0;
	iface.bInterfaceProtocol = 0;
	iface.iInterface = 0;

	ZeroInit(&data_endpoints_[0]);
	data_endpoints_[0].bLength = USB_DT_ENDPOINT_SIZE;
	data_endpoints_[0].bDescriptorType = USB_DT_ENDPOINT;
	data_endpoints_[0].bEndpointAddress = 0x01;
	data_endpoints_[0].bmAttributes = USB_ENDPOINT_ATTR_BULK;
	data_endpoints_[0].wMaxPacketSize = 64;
	data_endpoints_[0].bInterval = 1;

	ZeroInit(&data_endpoints_[1]);
	data_endpoints_[1].bLength = USB_DT_ENDPOINT_SIZE;
	data_endpoints_[1].bDescriptorType = USB_DT_ENDPOINT;
	data_endpoints_[1].bEndpointAddress = 0x82;
	data_endpoints_[1].bmAttributes = USB_ENDPOINT_ATTR_BULK;
	data_endpoints_[1].wMaxPacketSize = 64;
	data_endpoints_[1].bInterval = 1;

	iface.endpoint = data_endpoints_;
	return iface;
}

USBSerial::CDCFunctionalDescriptors USBSerial::GetCDCFunctionalDescriptors() {
	CDCFunctionalDescriptors func;
	ZeroInit(&func);

	func.header.bFunctionLength = sizeof(usb_cdc_header_descriptor);
	func.header.bDescriptorType = CS_INTERFACE;
	func.header.bDescriptorSubtype = USB_CDC_TYPE_HEADER;
	func.header.bcdCDC = 0x0110;

	func.call_mgmt.bFunctionLength = sizeof(usb_cdc_call_management_descriptor);
	func.call_mgmt.bDescriptorType = CS_INTERFACE;
	func.call_mgmt.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT;
	func.call_mgmt.bmCapabilities = 0;
	func.call_mgmt.bDataInterface = 1;

	func.acm.bFunctionLength = sizeof(usb_cdc_acm_descriptor);
	func.acm.bDescriptorType = CS_INTERFACE;
	func.acm.bDescriptorSubtype = USB_CDC_TYPE_ACM;
	func.acm.bmCapabilities = 0;

	func.cdc_union.bFunctionLength = sizeof(usb_cdc_union_descriptor);
	func.cdc_union.bDescriptorType = CS_INTERFACE;
	func.cdc_union.bDescriptorSubtype = USB_CDC_TYPE_UNION;
	func.cdc_union.bControlInterface = 0;
	func.cdc_union.bSubordinateInterface0 = 1;

	return func;
}

} // namespace Ostrich
