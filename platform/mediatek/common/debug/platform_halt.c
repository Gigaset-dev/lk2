/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <arch/arch_ops.h>
#include <debug.h>
#include <err.h>
#include <kernel/spinlock.h>
#include <list.h>
#include <malloc.h>
#include <platform.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <string.h>

struct halt_action {
    struct list_node node;
    const char *cb_name;
    void *data;
    status_t (*cb)(void *data,
                   platform_halt_action suggested_action,
                   platform_halt_reason reason);
};

struct callback_container_t {
    struct list_node cb_list;
    spin_lock_t cb_lock;
};

#define GEN_CB_CONTAINER(_name) \
{ \
    .cb_list = LIST_INITIAL_VALUE((_name).cb_list), \
    .cb_lock = SPIN_LOCK_INITIAL_VALUE,             \
}
static struct callback_container_t platform_halt_container =
GEN_CB_CONTAINER(platform_halt_container);

__WEAK int platform_power_hold(bool hold)
{
    return ERR_NOT_IMPLEMENTED;
}

__WEAK int platform_cold_reset(void)
{
    return ERR_NOT_IMPLEMENTED;
}

static void do_callback(struct callback_container_t *container,
                        platform_halt_action suggested_action,
                        platform_halt_reason reason)
{
    int ret;
    struct halt_action *action, *temp;
    spin_lock_saved_state_t state;

    spin_lock_irqsave(&container->cb_lock, state);
    list_for_every_entry_safe(&container->cb_list, action, temp, struct halt_action, node) {
        if (action->cb) {
            ret = action->cb(action->data, suggested_action, reason);
            dprintf(ALWAYS, "%s done\n", action->cb_name);
        }
        list_delete(&action->node);
        free(action);
    }
    spin_unlock_irqrestore(&container->cb_lock, state);
}

static status_t _register_callback(struct callback_container_t *container,
                                   const char *cb_name,
                                   status_t (*cb)(void *data,
                                                  platform_halt_action suggested_action,
                                                  platform_halt_reason reason),
                                   void *data)
{
    struct halt_action *action;
    spin_lock_saved_state_t state;

    action = (struct halt_action *)malloc(sizeof(struct halt_action));
    if (!action)
        return ERR_NO_MEMORY;

    memset(action, 0, sizeof(struct halt_action));
    list_initialize(&action->node);
    action->data = data;
    action->cb_name = cb_name;
    action->cb = cb;

    spin_lock_irqsave(&container->cb_lock, state);

    list_add_head(&container->cb_list, &action->node);

    spin_unlock_irqrestore(&container->cb_lock, state);
    return NO_ERROR;
}

status_t register_platform_halt_callback(
        const char *cb_name,
        status_t (*cb)(void *data,
                       platform_halt_action suggested_action,
                       platform_halt_reason reason),
        void *data)
{
    int ret;

    ret = _register_callback(&platform_halt_container, cb_name, cb, data);
    return ret;
}

void platform_halt(platform_halt_action suggested_action,
                   platform_halt_reason reason)
{
    int ret = 0;

    dprintf(ALWAYS, "HALT: action = %d, reason = %d\n", suggested_action, reason);

    do_callback(&platform_halt_container, suggested_action, reason);

    switch (suggested_action) {
    case HALT_ACTION_SHUTDOWN:
        ret = platform_power_hold(false);
        break;
    case HALT_ACTION_REBOOT:
        if (reason == HALT_REASON_POR) {
            ret = platform_cold_reset();
        } else
            mtk_wdt_reset(reason);
        break;
    default:
        break;
    }

    dprintf(ALWAYS, "HALT: spinning forever..., ret = %d\n", ret);
    arch_disable_ints();
    for (;;)
        ;
}
