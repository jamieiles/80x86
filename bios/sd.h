#pragma once

void sd_init(void);
void sd_boot(void);
int read_sector(unsigned short sector, unsigned short dseg,
                unsigned short daddr);
