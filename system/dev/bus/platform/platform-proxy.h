// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ddktl/device.h>
#include <ddktl/protocol/clk.h>
#include <ddktl/protocol/gpio.h>
#include <ddktl/protocol/i2c.h>
#include <ddktl/protocol/mailbox.h>
#include <ddktl/protocol/platform-device.h>
#include <ddktl/protocol/usb-mode-switch.h>
#include <lib/zx/handle.h>

namespace platform_bus {

class ProxyDevice;
using ProxyDeviceType = ddk::Device<ProxyDevice>;

class ProxyDevice : public ProxyDeviceType, public ddk::PdevProtocol<ProxyDevice>,
                    ddk::ClkProtocol<ProxyDevice>, ddk::GpioProtocol<ProxyDevice>,
                    ddk::I2cProtocol<ProxyDevice>, ddk::MailboxProtocol<ProxyDevice>,
                    ddk::UmsProtocol<ProxyDevice> {
public:
    explicit ProxyDevice(zx_device_t* parent, zx_handle_t rpc_channel)
        : ProxyDeviceType(parent), rpc_channel_(rpc_channel) {}

    zx_status_t Create(const char* name);

    void DdkRelease();

    // platform device protocol implementation
    zx_status_t MapMmio(uint32_t index, uint32_t cache_policy, void** out_vaddr, size_t* out_size,
                        zx_paddr_t* out_paddr, zx_handle_t* out_handle);
    zx_status_t MapInterrupt(uint32_t index, uint32_t flags, zx_handle_t* out_handle);
    zx_status_t GetBti(uint32_t index, zx_handle_t* out_handle);
    zx_status_t GetDeviceInfo(pdev_device_info_t* out_info);

    // clock protocol implementation
    zx_status_t ClkEnable(uint32_t index);
    zx_status_t ClkDisable(uint32_t index);

    // GPIO protocol implementation
    zx_status_t GpioConfig(uint32_t index, uint32_t flags);
    zx_status_t GpioSetAltFunction(uint32_t index, uint64_t function);
    zx_status_t GpioRead(uint32_t index, uint8_t* out_value);
    zx_status_t GpioWrite(uint32_t index, uint8_t value);
    zx_status_t GpioGetInterrupt(uint32_t index, uint32_t flags, zx_handle_t* out_handle);
    zx_status_t GpioReleaseInterrupt(uint32_t index);
    zx_status_t GpioSetPolarity(uint32_t index, uint32_t polarity);

    // I2C protocol implementation
    zx_status_t I2cTransact(uint32_t index, const void* write_buf, size_t write_length,
                            size_t read_length, i2c_complete_cb complete_cb, void* cookie);
    zx_status_t I2cGetMaxTransferSize(uint32_t index, size_t* out_size);

    // mailbox protocol implementation
    zx_status_t MailboxSendCmd(mailbox_channel_t* channel, mailbox_data_buf_t* mdata);

    // USB mode switch protocol implementation
    zx_status_t SetUsbMode(usb_mode_t mode);

private:
    DISALLOW_COPY_ASSIGN_AND_MOVE(ProxyDevice);

    zx::handle rpc_channel_;
};

} // namespace platform_bus

__BEGIN_CDECLS
zx_status_t platform_proxy_create(void* ctx, zx_device_t* parent, const char* name,
                                  const char* args, zx_handle_t rpc_channel);
__END_CDECLS
