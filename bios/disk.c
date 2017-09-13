#include "bios.h"
#include "sd.h"
#include "io.h"
#include "serial.h"
#include "utils.h"

#define SECTORS_PER_TRACK 63
#define NUM_HEADS 16
#define NUM_CYLINDERS 4
#define SECTOR_SIZE 512

#define FLOPPY_TIMEOUT 0x80

static void set_disk_status(struct callregs *regs, unsigned char err)
{
    if (err)
        regs->flags |= CF;
    else
        regs->flags &= ~CF;
    bda_write(diskette_status, err);
    regs->ax.h = err;
}

static void disk_read(struct callregs *regs)
{
    if (regs->dx.l != 0x80) {
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }

    unsigned short cylinder =
        regs->cx.h | (((unsigned short)regs->cx.l & 0xc0) << 2);
    unsigned short head = regs->dx.h;
    unsigned short sector = regs->cx.l & 0x3f;
    unsigned short lba =
        (cylinder * NUM_HEADS + head) * SECTORS_PER_TRACK + (sector - 1);
    unsigned short i;
    unsigned short dst = regs->bx.x;
    unsigned short count = regs->ax.l;

    regs->ax.l = 0;
    regs->flags &= ~CF;

    for (i = 0; i < count; ++i) {
        read_sector(lba, get_es(), dst);
        ++lba;
        dst += SECTOR_SIZE;
        ++regs->ax.l;
    }

    set_disk_status(regs, regs->ax.l != count ? 0xff : 0x00);
}

static void disk_write(struct callregs *regs)
{
    if (regs->dx.l != 0x80) {
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }

    unsigned short cylinder =
        regs->cx.h | (((unsigned short)regs->cx.l & 0xc0) << 2);
    unsigned short head = regs->dx.h;
    unsigned short sector = regs->cx.l & 0x3f;
    unsigned short lba =
        (cylinder * NUM_HEADS + head) * SECTORS_PER_TRACK + (sector - 1);
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
        dst += SECTOR_SIZE;
        ++regs->ax.l;
    }

    set_disk_status(regs, regs->ax.l != count ? 0xff : 0x00);
}

static void disk_status(struct callregs *regs)
{
    if (regs->dx.l != 0x80) {
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }

    unsigned char v = bda_read(diskette_status);
    bda_write(diskette_status, v);
}

static void disk_reset(struct callregs *regs)
{
    regs->flags &= ~CF;
    if (regs->dx.l != 0x80) {
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }
    set_disk_status(regs, 0);
}

static void disk_parameters(struct callregs *regs)
{
    regs->flags &= ~CF;
    if (regs->dx.l != 0x80) {
        regs->flags |= CF;
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }

    regs->dx.l = 1; // 1 drive
    regs->dx.h = NUM_HEADS - 1;
    regs->cx.x = SECTORS_PER_TRACK | (NUM_CYLINDERS << 8);
    regs->bx.l = 0;

    set_disk_status(regs, 0);
}

static void disk_get_type(struct callregs *regs)
{
    regs->flags &= ~CF;
    if (regs->dx.l != 0x80) {
        set_disk_status(regs, FLOPPY_TIMEOUT);
        return;
    }

    regs->ax.h = 3;
    regs->cx.x = regs->dx.x = 0xffff;
}

static void disk_services(struct callregs *regs)
{
    regs->flags |= CF;

    switch (regs->ax.h) {
    case 0x00: disk_reset(regs); break;
    case 0x01: disk_status(regs); break;
    case 0x02: disk_read(regs); break;
    case 0x03: disk_write(regs); break;
    case 0x08: disk_parameters(regs); break;
    case 0x15: disk_get_type(regs); break;
    }
}
VECTOR(0x13, disk_services);
