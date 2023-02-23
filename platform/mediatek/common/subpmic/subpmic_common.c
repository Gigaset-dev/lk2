#include <subpmic.h>
#include <trace.h>
#include <err.h>

#define LOCAL_TRACE 0

int __WEAK subpmic_init(void *fdt)
{
    LTRACEF("%s: not supported\n", __func__);
    return ERR_NOT_SUPPORTED;
}

int __WEAK subpmic_read_interface(enum subpmic_chan chan,
                                  u8 addr, u8 *data, u8 mask, u8 shift)
{
    LTRACEF("%s: not supported\n", __func__);
    return ERR_NOT_SUPPORTED;
}

int __WEAK subpmic_config_interface(enum subpmic_chan chan,
                                    u8 addr, u8 data, u8 mask, u8 shift)
{
    LTRACEF("%s: not supported\n", __func__);
    return ERR_NOT_SUPPORTED;
}

int __WEAK subpmic_block_read(enum subpmic_chan chan, u8 addr, u8 len, u8 *dst)
{
    LTRACEF("%s: not supported\n", __func__);
    return ERR_NOT_SUPPORTED;
}

int __WEAK subpmic_block_write(enum subpmic_chan chan, u8 addr, u8 len, u8 *src)
{
    LTRACEF("%s: not supported\n", __func__);
    return ERR_NOT_SUPPORTED;
}
