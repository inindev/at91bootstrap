/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "common.h"
#include "hardware.h"
#include "arch/at91_ccfg.h"
#include "arch/at91_rstc.h"
#include "arch/at91_pmc.h"
#include "arch/at91_pio.h"
#include "arch/at91_ddrsdrc.h"
#include "gpio.h"
#include "pmc.h"
#include "usart.h"
#include "debug.h"
#include "sdrsdramc.h"
#include "timer.h"
#include "watchdog.h"
#include "string.h"
#include "rth9580wf01.h"


static void at91_dbgu_hw_init(void)
{
	/* Configure DBGU pins */
	const struct pio_desc dbgu_pins[] = {
		{"RXD", AT91C_PIN_PA(9), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"TXD", AT91C_PIN_PA(10), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{(char *)0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	pmc_enable_periph_clock(AT91C_ID_PIOA_B);
	pio_configure(dbgu_pins);
}

static void initialize_dbgu(void)
{
	at91_dbgu_hw_init();
	usart_init(BAUDRATE(MASTER_CLOCK, BAUD_RATE));
}

/* using ISSI IS42S16800F */
static void sdrsdramc_reg_config(struct sdrsdramc_register *sdrsdramc_cfg)
{
	sdrsdramc_cfg->mdr = (AT91C_DDRC2_DBW_16_BITS
				| AT91C_DDRC2_MD_SDR_SDRAM);

	sdrsdramc_cfg->cr = (AT91C_DDRC2_NC_DDR10_SDR9            /* 9 column bits (512) */
				| AT91C_DDRC2_NR_12               /* 12 row bits (4K)    */
				| AT91C_DDRC2_CAS_2               /* CAS Latency 2       */
				| AT91C_DDRC2_NB_BANKS_4          /* 4 banks             */
				| AT91C_DDRC2_DISABLE_RESET_DLL
				| AT91C_DDRC2_DECOD_INTERLEAVED); /* interleaved decode  */

	/*
	 * SDRAM devices require a refresh of all rows every 64 ms. The value to be loaded depends
	 * on the DDRSDRC clock frequency (MCK: Master Clock) and the number of rows in the device.
	 *
	 *    64,000us / 4,096 rows = 15.625us
	 *    64ms / 4096 rows x MCK = (64/1000) / 4096 x MCK = MCK / 64,000
	 */
	sdrsdramc_cfg->rtr = MASTER_CLOCK / 64000;  /* 15.625us: 132,096,000 / 64,000 = 2064 */
	                                            /*       or:   15.625us x 132.096 = 2064 */

	/* IS42S16800F: 16MB (128Mb), 2Mb x16 x4 banks */
	/* one clock cycle @ 133 MHz = 7.5 ns               CAS Latency = 2      */
	sdrsdramc_cfg->t0pr = (AT91C_DDRC2_TRAS_(6)      /* 6 * 7.5 = 45ns  (37) */
				| AT91C_DDRC2_TRCD_(2)   /* 2 * 7.5 = 15ns  (15) */
				| AT91C_DDRC2_TWR_(2)    /* 2 * 7.5 = 15ns       */
				| AT91C_DDRC2_TRC_(8)    /* 8 * 7.5 = 60ns  (60) */
				| AT91C_DDRC2_TRP_(2)    /* 2 * 7.5 = 15ns  (15) */
				| AT91C_DDRC2_TRRD_(2)   /* 2 * 7.5 = 15ns  (14) */
				| AT91C_DDRC2_TMRD_(2)); /* 2 * 7.5 = 15ns  (14) */

	sdrsdramc_cfg->t1pr = (AT91C_DDRC2_TXP_(2)        /*  2 clock cycles     */
				| AT91C_DDRC2_TXSRD_(200) /* 200 clock cycles    */
				| AT91C_DDRC2_TXSNR_(19)  /* 19 * 7.5 = 142.5 ns */
				| AT91C_DDRC2_TRFC_(18)); /* 18 * 7.5 = 135 ns   */
}

static void sdrsdramc_init(void)
{
	unsigned long csa;
	struct sdrsdramc_register sdrsdramc_reg;

	sdrsdramc_reg_config(&sdrsdramc_reg);

	/* ENABLE DDR2 clock */
	pmc_enable_system_clock(AT91C_PMC_DDR);

	/* Chip select 1 is for DDR2/SDRAM */
	csa = readl(AT91C_BASE_CCFG + CCFG_EBICSA);
	csa |= AT91C_EBI_CS1A_SDRAMC;
	csa &= ~AT91C_EBI_DBPUC;
	csa |= AT91C_EBI_DBPDC;
	csa |= AT91C_EBI_DRV_HD;

	writel(csa, AT91C_BASE_CCFG + CCFG_EBICSA);

	/* SDR SDRAM controller initialize */
	sdrsdramc_initialize(AT91C_BASE_CS1, &sdrsdramc_reg);
}

#ifdef CONFIG_HW_INIT
void hw_init(void)
{
	/* Disable watchdog */
	at91_disable_wdt();

	/*
	 * At this stage the main oscillator is
	 * supposed to be enabled PCK = MCK = MOSC
	 */
	pmc_init_pll(0);

	/* Configure PLLA = MOSC * (PLL_MULA + 1) / PLL_DIVA */
	pmc_cfg_plla(PLLA_SETTINGS);

	/* Switch PCK/MCK on Main clock output */
	pmc_cfg_mck(BOARD_PRESCALER_MAIN_CLOCK);

	/* Switch PCK/MCK on PLLA output */
	pmc_cfg_mck(BOARD_PRESCALER_PLLA);

	/* Enable External Reset */
	writel(AT91C_RSTC_KEY_UNLOCK | AT91C_RSTC_URSTEN, AT91C_BASE_RSTC + RSTC_RMR);

	/* Init timer */
	timer_init();

	/* Initialize dbgu */
	initialize_dbgu();

	/* Initialize SDR SDRAM Controller */
	sdrsdramc_init();
}
#endif /* #ifdef CONFIG_HW_INIT */

#ifdef CONFIG_DATAFLASH
void at91_spi0_hw_init(void)
{
	/* Configure PINs for SPI0 */
	const struct pio_desc spi0_pins[] = {
		{"MISO",	AT91C_PIN_PA(11),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"MOSI",	AT91C_PIN_PA(12),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"SPCK",	AT91C_PIN_PA(13),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"NPCS",	CONFIG_SYS_SPI_PCS,	1, PIO_DEFAULT, PIO_OUTPUT},
		{(char *)0,	0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	pmc_enable_periph_clock(AT91C_ID_PIOA_B);
	pio_configure(spi0_pins);

	pmc_enable_periph_clock(AT91C_ID_SPI0);
}
#endif	/* #ifdef CONFIG_DATAFLASH */
