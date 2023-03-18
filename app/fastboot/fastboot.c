/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2019, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <assert.h>
#include <debug.h>
#include <dev/udc.h>
#include <err.h>
#include <fastboot.h>
#include <kernel/event.h>
#include <kernel/spinlock.h>
#include <kernel/timer.h>
#include <list.h>
#include <lock_state.h>
#include <platform.h>
#include <platform_mtk.h>
#include <sboot.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <verified_boot_state.h>

/* Current write cmd timeout of USB and PCIe are the same, so define it here.
 * If the value of new added controller is different, then consider to extend
 * the fastboot_controller_interface structure.
 */
#define WRITE_CMD_TIMEOUT 5000

#define LOCAL_TRACE 2

static timer_t fastboot_idle_timer;
static lk_time_t fastboot_idle_timeout;

struct fastboot_cmd {
    struct fastboot_cmd *next;
    const char *prefix;
    unsigned int prefix_len;
    int allowed_when_security_on;
    int forbidden_when_lock_on;
    void (*handle)(const char *arg, void *data, unsigned int sz);
};

void *download_base;
unsigned int download_max;
unsigned int download_size;
unsigned int fastboot_state = STATE_OFFLINE;

struct fastboot_cmd *cmdlist;
struct fastboot_var *varlist;
struct fastboot_controller_interface *interface;

static struct {
    struct list_node list;
    spin_lock_t lock;
} interfaces = {
    .list = LIST_INITIAL_VALUE(interfaces.list),
    .lock = SPIN_LOCK_INITIAL_VALUE,
};

struct controller_interface {
    struct list_node node;
    struct fastboot_controller_interface *interface;
};

status_t register_fastboot_controller_interface(struct fastboot_controller_interface *interface)
{
    spin_lock_saved_state_t state;

    struct controller_interface
        *ci = (struct controller_interface *)malloc(sizeof(struct controller_interface));

    if (!ci)
        return ERR_NO_MEMORY;

    list_initialize(&ci->node);
    ci->interface = interface;

    spin_lock_irqsave(&interfaces.lock, state);

    list_add_head(&interfaces.list, &ci->node);

    spin_unlock_irqrestore(&interfaces.lock, state);

    return NO_ERROR;
}

static void unregister_fastboot_controller_interfaces(void)
{
    struct controller_interface *ci, *temp;
    spin_lock_saved_state_t state;

    spin_lock_irqsave(&interfaces.lock, state);
    list_for_every_entry_safe(&interfaces.list, ci, temp, struct controller_interface, node) {
        free(ci);
    }
    spin_unlock_irqrestore(&interfaces.lock, state);
}

__WEAK const char *get_fastboot_controller_interface_name(void)
{
    static const char *name = "usb";

    return name;
}

static struct fastboot_controller_interface *fastboot_get_controller_interface(const char *name)
{
    struct controller_interface *ci, *temp;
    spin_lock_saved_state_t state;

    spin_lock_irqsave(&interfaces.lock, state);
    list_for_every_entry_safe(&interfaces.list, ci, temp, struct controller_interface, node) {
        if (strncmp(ci->interface->name, name, strlen(ci->interface->name)) == 0) {
            spin_unlock_irqrestore(&interfaces.lock, state);
            return ci->interface;
        }
    }
    spin_unlock_irqrestore(&interfaces.lock, state);

    return NULL;
}

unsigned int fastboot_get_state(void)
{
    return fastboot_state;
}

void fastboot_set_state(unsigned int state)
{
    fastboot_state = state;
}

void fastboot_unregister_all(void)
{
    struct fastboot_cmd *cmd;

    while (cmdlist != NULL) {
        cmd = cmdlist;
        cmdlist = cmd->next;
        free(cmd);
    }
}

void fastboot_register(const char *prefix,
                       void (*handle)(const char *arg, void *data, unsigned int sz),
                       int allowed_when_security_on, int forbidden_when_lock_on)
{
    struct fastboot_cmd *cmd;

    cmd = malloc(sizeof(*cmd));
    if (cmd) {
        cmd->prefix = prefix;
        cmd->prefix_len = strlen(prefix);
        cmd->allowed_when_security_on = allowed_when_security_on;
        cmd->forbidden_when_lock_on = forbidden_when_lock_on;
        cmd->handle = handle;
        cmd->next = cmdlist;
        cmdlist = cmd;
    }
}

void fastboot_unpublish_all(void)
{
    struct fastboot_var *var;

    while (varlist != NULL) {
        var = varlist;
        if (var->name)
            free((void *)var->name);
        if (var->value)
            free((void *)var->value);
        varlist = var->next;
        free(var);
    }
}

void fastboot_publish(const char *name, const char *value)
{
    struct fastboot_var *var;
    char *nm, *val;

    if (!value) {
        if (name)
            dprintf(INFO, "publish %s fails due to null value.\n", name);
        return;
    }

    nm = strdup(name);
    val = strdup(value);
    var = malloc(sizeof(*var));
    if (var && nm && val) {
        var->name = (const char *)nm;
        var->value = (const char *)val;
        var->next = varlist;
        varlist = var;
    } else {
        if (var)
            free(var);
        if (nm)
            free(nm);
        if (val)
            free(val);
    }
}

void fastboot_update_var(const char *name, const char *value)
{
    struct fastboot_var *var = varlist;

    while (var != NULL) {
        if (!strcmp(name, var->name))
            var->value = value;

        var = var->next;
    }
}

const char *fastboot_get_var(const char *name)
{
    struct fastboot_var *var = varlist;

    while (var != NULL) {
        if (!strcmp(name, var->name))
            return var->value;
        var = var->next;
    }

    return "";
}

void fastboot_ack(const char *code, const char *reason)
{
    char *response;

    if (fastboot_state != STATE_COMMAND)
        return;

    if (reason == 0)
        reason = "";

    response = (char *)memalign(CACHE_LINE, MAX_RSP_SIZE);
    if (!response) {
        dprintf(CRITICAL, "fastboot: can't allocate memory\n");
        return;
    }
    snprintf(response, MAX_RSP_SIZE, "%s%s", code, reason);
    fastboot_state = STATE_COMPLETE;

    interface->write_with_timeout(response, strlen(response), WRITE_CMD_TIMEOUT);
    free(response);

}

void fastboot_info(const char *reason)
{
    char *response;

    if (fastboot_state != STATE_COMMAND)
        return;

    if (reason == 0)
        return;

    response = (char *)memalign(CACHE_LINE, MAX_RSP_SIZE);
    if (!response) {
        dprintf(CRITICAL, "fastboot: can't allocate memory\n");
        return;
    }
    snprintf(response, MAX_RSP_SIZE, "INFO%s", reason);

    interface->write_with_timeout(response, strlen(response), WRITE_CMD_TIMEOUT);
    free(response);
}

void fastboot_fail(const char *reason)
{
    fastboot_ack("FAIL", reason);
    /* add debug log */
    dprintf(ALWAYS, "%s\n", reason);
}

void fastboot_okay(const char *info)
{
    fastboot_ack("OKAY", info);
}

void fastboot_idle_reboot(void)
{
    dprintf(CRITICAL, "fastboot mode idle for a while. Trigger system reboot now...\n");
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_POR);
}

static int fastboot_command_loop(void)
{
    struct fastboot_cmd *cmd;
    int r;
    char *buffer = (char *)memalign(CACHE_LINE, MAX_RSP_SIZE);

    if (!buffer) {
        dprintf(CRITICAL, "fastboot: can't allocate memory\n");
        return ERR_NO_MEMORY;
    }

again:
    while ((fastboot_state != STATE_ERROR) && (fastboot_state != STATE_RETURN)) {
        if (fastboot_idle_timeout > 0) { /* set fastboot idle timer */
            timer_set_periodic(&fastboot_idle_timer, fastboot_idle_timeout,
                (timer_callback)fastboot_idle_reboot, NULL);
        }
        dprintf(ALWAYS, "fastboot: waiting commands\n");

        r = interface->read(buffer, MAX_RSP_SIZE);
        if ((strncmp(interface->name, "usb", strlen(interface->name)) == 0) && r < 0)
            break; /* no input command */
        else if ((strncmp(interface->name, "pcie", strlen(interface->name)) == 0) && r <= 0)
            continue; /* no data after RX timeout, keep receiving */

        if (fastboot_idle_timeout > 0) { /* RX data received, cancel idle timer */
            timer_cancel(&fastboot_idle_timer);
        }
        buffer[r] = 0;
        dprintf(ALWAYS, "fastboot: %s[len:%d]\n", buffer, r);
        dprintf(ALWAYS, "fastboot:[download_base:%p][download_size:0x%x]\n",
                download_base, (unsigned int)download_size);

        /* Pick up matched command and handle it */
        for (cmd = cmdlist; cmd; cmd = cmd->next) {
            fastboot_state = STATE_COMMAND;
            if (memcmp(buffer, cmd->prefix, cmd->prefix_len))
                continue;
            dprintf(ALWAYS, "fastboot:[cmd:%s]-[arg:%s]\n", cmd->prefix, buffer + cmd->prefix_len);

#if (MTK_SECURITY_SW_SUPPORT)
            uint32_t sboot_state = 1;
            uint32_t lock_state = LKS_LOCK;

            get_sboot_state(&sboot_state);
            sec_get_lock_state_adapter(&lock_state);

            if (((sboot_state == 0) || cmd->allowed_when_security_on)
                    && ((!cmd->forbidden_when_lock_on)) || lock_state == LKS_UNLOCK) {
                cmd->handle((const char *) buffer + cmd->prefix_len, (void *) download_base,
                           download_size);
            }

            if (fastboot_state == STATE_COMMAND) {
                if ((sboot_state == 1) && !cmd->allowed_when_security_on)
                    fastboot_fail("No support by security control");
                else if (((cmd->forbidden_when_lock_on)) && lock_state != LKS_UNLOCK)
                    fastboot_fail("No support by lock control");
                else
                    fastboot_fail("unknown reason");
            }
#else
            cmd->handle((const char *) buffer + cmd->prefix_len, (void *) download_base,
                       download_size);
            if (fastboot_state == STATE_COMMAND)
                fastboot_fail("unknown reason");
#endif

            goto again;
        }
        dprintf(ALWAYS, "[unknown command]*[%s]*\n", buffer);
        fastboot_fail("unknown command");

    }
    if (fastboot_state != STATE_RETURN)
        fastboot_state = STATE_OFFLINE;
    dprintf(ALWAYS, "fastboot: oops!\n");
    free(buffer);
    return fastboot_state;
}

static int fastboot_handler(void)
{
    int status = 0;

    while (status != STATE_RETURN) {
        if (strncmp(interface->name, "usb", strlen(interface->name)) == 0)
            interface->event_wait();

        status = fastboot_command_loop();
    }
    return 0;
}

/* Set state to STATE_RETURN to exit fastboot_handler() */
void fastboot_exit(void)
{
    fastboot_state = STATE_RETURN;
    dprintf(CRITICAL, "fastboot: set state to STATE_RETURN\n");
}

int fastboot_init(void *base, unsigned int size)
{
    int timeout_value;
    const char *name;

    LTRACEF("%s()\n", __func__);
    download_base = base;
    download_max = size;
    timeout_value = get_fastboot_idle_timeout();
    if (timeout_value > 0) {
        fastboot_idle_timeout = timeout_value;
        timer_initialize(&fastboot_idle_timer);
        LTRACEF("Init. fastboot idle timer. Timeout value:%d ms\n", fastboot_idle_timeout);
    }

    name = get_fastboot_controller_interface_name();
    ASSERT(name);

    interface = fastboot_get_controller_interface(name);
    ASSERT(interface);

    if (interface->init() < 0)
        return -1;

    interface->start();

    fastboot_handler();

    interface->stop();

    interface->fini();

    unregister_fastboot_controller_interfaces();

    return 0;
}
