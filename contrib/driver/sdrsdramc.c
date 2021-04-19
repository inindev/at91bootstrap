#include "hardware.h"
#include "arch/at91_ddrsdrc.h"
#include "debug.h"
#include "sdrsdramc.h"
#include "timer.h"


#define write_sdrsdramc(reg, value) \
	(*(volatile unsigned int *)((reg) + AT91C_BASE_DDRSDRC)) = (value)

/* see 29.4.1 SDR-SDRAM Initialization, SAM9G35 datasheet */
void sdrsdramc_initialize(unsigned int ram_address, struct sdrsdramc_register *sdrsdramc_cfg)
{
	unsigned int i;

	/*
	 * step 1: program the memory device type in the DDRSDRC Memory Device Register
	 */
	write_sdrsdramc(HDDRSDRC2_MDR, sdrsdramc_cfg->mdr);

	/*
	 * step 2: program features of the SDR-SDRAM device in the DDRSDRC Configuration
	 *         Register and in the DDRSDRC Timing Parameter Registers
	 */
	write_sdrsdramc(HDDRSDRC2_CR, sdrsdramc_cfg->cr);
	write_sdrsdramc(HDDRSDRC2_T0PR, sdrsdramc_cfg->t0pr);
	write_sdrsdramc(HDDRSDRC2_T1PR, sdrsdramc_cfg->t1pr);

	/* step 3: a minimum pause of 200 μs is provided to precede any signal toggle */
	udelay(200);

	/*
	 * step 4: issue a NOP command to the SDR-SDRAM and perform a write
	 *         access to any SDR-SDRAM address to acknowledge this command
	 */
	write_sdrsdramc(HDDRSDRC2_MR, AT91C_DDRC2_MODE_NOP_CMD);
	writel(0x00000000, ram_address);

	/* the clock which drives SDR-SDRAM device is now enabled */

	/*
	 * step 5: issue an All Banks Precharge command to the SDR-SDRAM and perform
	 *         a write access to any SDR-SDRAM address to acknowledge this command
	 */
	write_sdrsdramc(HDDRSDRC2_MR, AT91C_DDRC2_MODE_PRCGALL_CMD);
	writel(0x00000000, ram_address);

	/*
	 * step 6: issue eight CAS before RAS (CBR) auto-refresh cycles to program CBR,
	 *         configure the MODE field value to 4 in the DDRSDRC Mode Register and
	 *         perform a write access to any SDR-SDRAM location eight times to
	 *         acknowledge these commands
	 */
	for(i = 0; i < 8; i++) {
		write_sdrsdramc(HDDRSDRC2_MR, AT91C_DDRC2_MODE_RFSH_CMD);
		writel(0x00000000, ram_address);
	}

	/*
	 * step 7: a Mode Register set (MRS) cycle is issued to program the parameters
	 *         of the SDR-SDRAM devices (in particular CAS latency and burst length)
	 *         to program MRS, configure the MODE field value to 3 in the DDRSDRC
	 *         Mode Register and perform a write access such that BA[1:0] are set to 0
	 */
	write_sdrsdramc(HDDRSDRC2_MR, AT91C_DDRC2_MODE_LMR_CMD);
	writel(0x00000000, ram_address);

	/*
	 * step 8: go into Normal mode by configuring the MODE field value to 0 in the
	 *         DDRSDRC Mode Register and performing a write access to any SDR-SDRAM
	 *         address to acknowledge this command
	 */
	write_sdrsdramc(HDDRSDRC2_MR, AT91C_DDRC2_MODE_NORMAL_CMD);
	writel(0x00000000, ram_address);

	/*
	 * step 9: write the refresh rate into the COUNT field of the
	 *         DDRSDRC Refresh Timer Register
	 */
	write_sdrsdramc(HDDRSDRC2_RTR, sdrsdramc_cfg->rtr);

	/* the SDR-SDRAM device is now fully functional */
}

void ddramc_dump_regs(unsigned int base_address)
{
#if (BOOTSTRAP_DEBUG_LEVEL >= DEBUG_LOUD)
        unsigned int size = 0x160;

        dbg_info("\nDump DDRAMC Registers:\n");
        dbg_hexdump((unsigned char *)base_address, size, DUMP_WIDTH_BIT_32);
#endif
}

