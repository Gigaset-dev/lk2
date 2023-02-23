/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum DISP_PATH_EVENT {
    DISP_PATH_EVENT_FRAME_START = 0,
    DISP_PATH_EVENT_FRAME_DONE,
    DISP_PATH_EVENT_FRAME_REG_UPDATE,
    DISP_PATH_EVENT_FRAME_TARGET_LINE,
    DISP_PATH_EVENT_FRAME_COMPLETE,
    DISP_PATH_EVENT_FRAME_STOP,
    DISP_PATH_EVENT_IF_CMD_DONE,
    DISP_PATH_EVENT_IF_VSYNC,
    DISP_PATH_EVENT_AAL_TRIGGER,
    DISP_PATH_EVENT_COLOR_TRIGGER,
    DISP_PATH_EVENT_NUM,
    DISP_PATH_EVENT_NONE = 0xff,
};
