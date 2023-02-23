/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum snfi_ctrl_op_mode {
    SNFI_CUSTOM_MODE,
    SNFI_AUTO_MODE,
    SNFI_MAC_MODE
};

enum snfi_ctrl_rx_mode {
    SNFI_RX_111,
    SNFI_RX_112,
    SNFI_RX_114,
    SNFI_RX_122,
    SNFI_RX_144
};

enum snfi_ctrl_tx_mode {
    SNFI_TX_111,
    SNFI_TX_114,
};

enum chip_ctrl_drive_strength {
    CHIP_DRIVE_NORMAL,
    CHIP_DRIVE_HIGH,
    CHIP_DRIVE_MIDDLE,
    CHIP_DRIVE_LOW
};

enum chip_ctrl_timing_mode {
    CHIP_TIMING_MODE0,
    CHIP_TIMING_MODE1,
    CHIP_TIMING_MODE2,
    CHIP_TIMING_MODE3,
    CHIP_TIMING_MODE4,
    CHIP_TIMING_MODE5,
};


