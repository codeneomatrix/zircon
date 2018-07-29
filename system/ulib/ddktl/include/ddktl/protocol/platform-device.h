// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ddk/protocol/platform-device.h>
#include <ddktl/device-internal.h>
#include <zircon/assert.h>

#include "platform-device-internal.h"

// DDK platform device protocol support.
//
// :: Proxies ::
//
// ddk::PdevProtocolProxy is a simple wrappers around platform_device_protocol_t. It does
// not own the pointers passed to it.
//
// :: Mixins ::
//
// ddk::PdevProtocol is a mixin class that simplifies writing DDK drivers that
// implement the platform device protocol.
//
// :: Examples ::
//
// // A driver that implements a ZX_PROTOCOL_PLATFORM_DEV device.
// class PdevDevice;
// using PdevDeviceType = ddk::Device<PdevDevice, /* ddk mixins */>;
//
// class PdevDevice : public PdevDeviceType,
//                    public ddk::PdevProtocol<PdevDevice> {
//   public:
//     PdevDevice(zx_device_t* parent)
//       : PdevDeviceType("my-platform-device", parent) {}
//
//        zx_status_t MapMmio(uint32_t index, uint32_t cache_policy, void** out_vaddr, size_t* out_size,
//                            zx_paddr_t* out_paddr, zx_handle_t* out_handle);
//        zx_status_t MapInterrupt(uint32_t index, uint32_t flags, zx_handle_t* out_handle);
//        zx_status_t GetBti(uint32_t index, zx_handle_t* out_handle);
//        zx_status_t GetDeviceInfo(pdev_device_info_t* out_info);
//     ...
// };

namespace ddk {

template <typename D>
class PdevProtocol : public internal::base_protocol {
public:
    PdevProtocol() {
        internal::CheckPdevProtocolSubclass<D>();
        pdev_proto_ops_.map_mmio = MapMmio;
        pdev_proto_ops_.map_interrupt = MapInterrupt;
        pdev_proto_ops_.get_bti = GetBti;
        pdev_proto_ops_.get_device_info = GetDeviceInfo;

        // Can only inherit from one base_protocol implementation.
        ZX_ASSERT(ddk_proto_id_ == 0);
        ddk_proto_id_ = ZX_PROTOCOL_PLATFORM_DEV;
        ddk_proto_ops_ = &pdev_proto_ops_;
    }

protected:
    platform_device_protocol_ops_t pdev_proto_ops_ = {};

private:
    static zx_status_t MapMmio(void* ctx, uint32_t index, uint32_t cache_policy, void** out_vaddr,
                        size_t* out_size, zx_paddr_t* out_paddr, zx_handle_t* out_handle) {
        return static_cast<D*>(ctx)->MapMmio(index, cache_policy, out_vaddr, out_size, out_paddr,
                                             out_handle);
    }

    static zx_status_t MapInterrupt(void* ctx, uint32_t index, uint32_t flags, zx_handle_t* out_handle) {
        return static_cast<D*>(ctx)->MapInterrupt(index, flags, out_handle);
    }

    static zx_status_t GetBti(void* ctx, uint32_t index, zx_handle_t* out_handle) {
        return static_cast<D*>(ctx)->GetBti(index, out_handle);
    }

    static zx_status_t GetDeviceInfo(void* ctx, pdev_device_info_t* out_info) {
        return static_cast<D*>(ctx)->GetDeviceInfo(out_info);
    }
};

class PdevProtocolProxy {
public:
    PdevProtocolProxy(platform_device_protocol_t* proto)
        : ops_(proto->ops), ctx_(proto->ctx) {}

    zx_status_t MapMmio(uint32_t index, uint32_t cache_policy, void** out_vaddr,
                        size_t* out_size, zx_paddr_t* out_paddr, zx_handle_t* out_handle) {
        return ops_->map_mmio(ctx_, index, cache_policy, out_vaddr, out_size, out_paddr, out_handle);
    }

    zx_status_t MapInterrupt(uint32_t index, uint32_t flags, zx_handle_t* out_handle) {
        return ops_->map_interrupt(ctx_, index, flags, out_handle);
    }

    zx_status_t GetBti(uint32_t index, zx_handle_t* out_handle) {
        return ops_->get_bti(ctx_, index, out_handle);
    }

    zx_status_t GetDeviceInfo(pdev_device_info_t* out_info) {
        return ops_->get_device_info(ctx_, out_info);
    }

private:
    platform_device_protocol_ops_t* ops_;
    void* ctx_;
};

} // namespace ddk
