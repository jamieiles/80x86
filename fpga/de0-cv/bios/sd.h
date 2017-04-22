#pragma once

void sd_init(void);
void sd_boot(void);
int read_sector(unsigned long address, unsigned short dseg,
                unsigned short daddr);
