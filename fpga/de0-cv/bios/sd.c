#include "io.h"
#include "serial.h"
#include "bios.h"

#define SPI_CONTROL_PORT 0xfff0
#define SPI_TRANSFER_PORT 0xfff2
#define SPI_XFER_BUSY (1 << 8)
#define SPI_CS_DEACTIVATE (1 << 9)

static char spi_xfer_buf[64];

static void spi_xfer_buf_set(int offs, unsigned char v)
{
    write_csbyte(spi_xfer_buf + offs, v);
}

static unsigned char spi_xfer_buf_get(int offs)
{
    return read_csbyte(spi_xfer_buf + offs);
}

static void spi_wait_idle(void)
{
    while (inw(SPI_TRANSFER_PORT) & SPI_XFER_BUSY)
        continue;
}

static void spi_xfer(int len)
{
    int i;

    for (i = 0; i < len; ++i) {
        outw(SPI_TRANSFER_PORT, spi_xfer_buf_get(i));
        spi_wait_idle();
        spi_xfer_buf_set(i, inw(SPI_TRANSFER_PORT) & 0xff);
    }
}

static void sd_send_initial_clock(void)
{
    int i;

    for (i = 0; i < 8; ++i)
        spi_xfer_buf_set(i, 0xff);
    // No CS, slow clock
    outw(SPI_CONTROL_PORT, SPI_CS_DEACTIVATE | 0x1ff);
    
    spi_xfer(8);
}

static void sd_flush_fifo(void)
{
    int i;

    for (i = 0; i < 128; ++i)
        spi_xfer_buf_set(i, 0xff);
    // No CS, slow clock
    outw(SPI_CONTROL_PORT, 0x1ff);
    
    spi_xfer(8);
}

struct spi_cmd {
    unsigned char cmd;
    unsigned char arg[4];
    unsigned char crc;

    const unsigned char *data;
    unsigned short tx_datalen;
    unsigned short rx_datalen;
};

struct r1_response {
    unsigned char v;
};

#define R1_ERROR_MASK 0xfe
#define SD_NCR		8
#define DATA_START_TOKEN	0xfe
#define BLOCK_SIZE		512
/* Maximum number of high bytes to read before a data start token. */
#define MAX_DATA_START_OFFS 512

static unsigned short spi_command_len(const struct spi_cmd *cmd)
{
    return 1 + 6 + cmd->tx_datalen + cmd->rx_datalen + SD_NCR;
}

static void spi_do_command(const struct spi_cmd *cmd)
{
    int m, cmdlen;

    cmdlen = spi_command_len(cmd);

    // The command.
    spi_xfer_buf_set(0, 0xff);
    spi_xfer_buf_set(1, cmd->cmd);
    for (m = 0; m < 4; ++m)
        spi_xfer_buf_set(2 + m, cmd->arg[m]);
    spi_xfer_buf_set(6, cmd->crc);
    // Transmit data.
    for (m = 0; m < cmd->tx_datalen; ++m)
        spi_xfer_buf_set(7 + m, cmd->data[m]);
    // Initialize receive buffer so we don't shift out new, garbage data.
    for (m = 7 + cmd->tx_datalen; m < cmdlen; ++m)
        spi_xfer_buf_set(m, 0xff);

    // Fast clock, CS enabled.
    outw(SPI_CONTROL_PORT, 0x8);
    spi_xfer(cmdlen);
}

static int find_r1_response(struct r1_response *r1)
{
    int i;

    r1->v = 0;

    for (i = 0; i < sizeof(spi_xfer_buf) / sizeof(spi_xfer_buf[0]); ++i)
        if (spi_xfer_buf_get(i) != 0xff) {
            r1->v = spi_xfer_buf_get(i);
            break;
        }

    if (i == sizeof(spi_xfer_buf))
        return -1;

    return i;
}

static int sd_send_reset(void)
{
    struct spi_cmd cmd = {
        .cmd = 0x40,
        .crc = 0x95,
        .rx_datalen = 1,
    };
    struct r1_response r1;

    spi_do_command(&cmd);
    if (find_r1_response(&r1) < 0)
        return -1;

    return r1.v & R1_ERROR_MASK;
}

static int sd_send_if_cond(void)
{
    struct spi_cmd cmd = {
        .cmd = 0x48,
        .crc = 0x87,
        .arg = { 0x00, 0x00, 0x01, 0xaa },
        .rx_datalen = 1,
    };
    struct r1_response r1;

    spi_do_command(&cmd);
    if (find_r1_response(&r1) < 0)
        return -1;

    return r1.v & R1_ERROR_MASK;
}

static int sd_send_read_ocr(void)
{
    struct spi_cmd cmd = {
        .cmd = 0x7a,
        .rx_datalen = 5,
    };
    struct r1_response r1;

    spi_do_command(&cmd);
    if (find_r1_response(&r1) < 0)
        return -1;

    return r1.v & R1_ERROR_MASK;
}

static int send_acmd(void)
{
    struct spi_cmd cmd = {
        .cmd = 0x77,
        .arg = { 0x00, 0x00, 0x00, 0x00 },
        .rx_datalen = 1,
    };
    struct r1_response r1;

    spi_do_command(&cmd);
    if (find_r1_response(&r1) < 0)
        return -1;

    return r1.v & R1_ERROR_MASK;
}

static int sd_wait_ready(void)
{
    struct r1_response r1 = {};

    do {
        struct spi_cmd cmd = {
            .cmd = 0x69,
            .arg = { 0x40, 0x00, 0x00, 0x00 },
            .rx_datalen = 1,
        };
        int rc = send_acmd();

        if (rc)
            return rc;

        spi_do_command(&cmd);
        if (find_r1_response(&r1) < 0)
            return -1;

        if (r1.v & R1_ERROR_MASK)
            return r1.v & R1_ERROR_MASK;
    } while (r1.v & 0x1);

    return 0;
}

static int sd_set_blocklen(void)
{
    struct spi_cmd cmd = {
        .cmd = 0x50,
        /* BLOCK_SIZE bytes */
        .arg = { 0x00, 0x00, 0x02, 0x00 },
        .rx_datalen = 1,
    };
    struct r1_response r1;

    spi_do_command(&cmd);
    if (find_r1_response(&r1) < 0)
        return -1;

    return r1.v & R1_ERROR_MASK;
}

static int find_data_start(int r1offs)
{
    ++r1offs;
    while (spi_xfer_buf_get(r1offs) != DATA_START_TOKEN &&
           r1offs < sizeof(spi_xfer_buf))
        ++r1offs;

    return r1offs == sizeof(spi_xfer_buf) ? -1 : r1offs + 1;
}

int read_sector(unsigned long address, unsigned short dseg,
                unsigned short daddr)
{
    struct spi_cmd cmd = {
        .cmd = 0x51,
        /* r1, start token, data, CRC16 */
        .rx_datalen = 1 + 1 + BLOCK_SIZE + 2 + MAX_DATA_START_OFFS,
    };
    struct r1_response r1;
    int r1offs, data_start;

    cmd.arg[0] = (address >> 24) & 0xff;
    cmd.arg[1] = (address >> 16) & 0xff;
    cmd.arg[2] = (address >> 8)  & 0xff;
    cmd.arg[3] = (address >> 0)  & 0xff;

    spi_do_command(&cmd);
    r1offs = find_r1_response(&r1);
    if (r1offs < 0) {
        putstr("Failed to find R1 response\n\r");
        return -1;
    }
    if (r1.v & R1_ERROR_MASK) {
        putstr("Read sector failed\n\r");
        return -1;
    }

    data_start = find_data_start(r1offs);
    if (data_start < 0)
        return -1;

    memcpy_seg(dseg, (void *)daddr, get_cs(), spi_xfer_buf + data_start,
               512);

    return 0;
}

void sd_init(void)
{
    sd_send_initial_clock();
    sd_flush_fifo();
    if (sd_send_reset())
        panic("Failed to reset SD card");
    if (sd_send_if_cond())
        panic("Failed to send interface conditions to SD card");
    if (sd_send_read_ocr())
        panic("Failed to read OCR for SD card");
    if (sd_wait_ready())
        panic("Failed to wait for SD card init");
    if (sd_set_blocklen())
        panic("Failed to wait for SD card init");

    putstr("SD card ready\r\n");
}

void sd_boot(void)
{
    read_sector(0, 0, 0x7c00);

    putstr("Booting from SD card...\n\r");
    asm volatile("mov $0, %dl\n"
                 "jmp $0x0000, $0x7c00");
}
