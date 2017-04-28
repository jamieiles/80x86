#include "bios.h"
#include "sd.h"
#include "io.h"
#include "serial.h"

static void disk_read(struct callregs *regs)
{
    if (regs->dx.l != 0) {
        regs->ax.l = 0x80;
        return;
    }

    unsigned short cylinder = regs->cx.h |
        (((unsigned short)regs->cx.l & 0xc0) << 2);
    unsigned short head = regs->dx.h;
    unsigned short sector = regs->cx.l & 0x3f;
    unsigned short lba = (cylinder * 2 + head) * 0x12 + (sector - 1);
    unsigned short i;
    unsigned short dst = regs->bx.x;
    unsigned short count = regs->ax.l;

    regs->ax.l = 0;
    regs->flags &= ~CF;

    for (i = 0; i < count; ++i) {
        read_sector(lba, get_es(), dst);
        ++lba;
        dst += 512;
        ++regs->ax.l;
    }

    regs->ax.h = regs->ax.l != count ? 0x20 : 0x00;
}

static void disk_write(struct callregs *regs)
{
    if (regs->dx.l != 0) {
        regs->ax.l = 0x80;
        return;
    }

    unsigned short cylinder = regs->cx.h |
        (((unsigned short)regs->cx.l & 0xc0) << 2);
    unsigned short head = regs->dx.h;
    unsigned short sector = regs->cx.l & 0x3f;
    unsigned short lba = (cylinder * 2 + head) * 0x12 + (sector - 1);
    unsigned short i;
    unsigned short dst = regs->bx.x;
    unsigned short count = regs->ax.l;

    regs->ax.l = 0;
    regs->flags &= ~CF;

    for (i = 0; i < count; ++i) {
        if (write_sector(lba, get_es(), dst)) {
            regs->flags |= CF;
            break;
        }
        ++lba;
        dst += 512;
        ++regs->ax.l;
    }

    regs->ax.h = regs->ax.l != count ? 0x20 : 0x00;
}

static void disk_status(struct callregs *regs)
{
    if (regs->dx.l != 0)
        return;

    regs->flags &= ~CF;
    regs->ax.h = 0;
}

static void disk_reset(struct callregs *regs)
{
    if (regs->dx.l != 0)
        return;
    regs->flags &= ~CF;
    regs->ax.h = 0;
}

static unsigned char disk_base_table[] = {
    0, // 0 step-rate
    0, // 1 head load time
    0, // 2 ticks to shutoff
    2, // 3 512 bytes per secto
    0, // 4 1 sector per track
    0, // 5 inter block gap
    0, // 6 data length
    0, // 7 gap length
    0, // 8 fill byte
    0, // 9 head settle
    0, // a motor startup time
};

static void disk_parameters(struct callregs *regs)
{
    if (regs->dx.l != 0)
        return;

    regs->flags &= ~CF;
    regs->ax.h = 0;
    regs->dx.l = 1; // 1 drive
    regs->dx.h = 0; // 1 head
    regs->bx.l = 4;
    regs->di.x = (unsigned short)disk_base_table;
    set_es(get_cs());
}

static void disk_get_type(struct callregs *regs)
{
    if (regs->dx.l != 0)
        return;

    regs->ax.h = 1;
    regs->flags &= ~CF;
}

void int13_function(struct callregs *regs)
{
    regs->flags |= CF;

    switch (regs->ax.h) {
    case 0x00:
        disk_reset(regs);
        break;
    case 0x01:
        disk_status(regs);
        break;
    case 0x02:
        disk_read(regs);
        break;
    case 0x03:
        disk_write(regs);
        break;
    case 0x08:
        disk_parameters(regs);
        break;
    case 0x15:
        disk_get_type(regs);
        break;
    }
}
