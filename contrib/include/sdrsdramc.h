#ifndef __SDRSDRAMC_H__
#define __SDRSDRAMC_H__

struct sdrsdramc_register {
	unsigned int mdr;
	unsigned int cr;
	unsigned int rtr;
	unsigned int t0pr;
	unsigned int t1pr;
};

extern void sdrsdramc_initialize(unsigned int ram_address,
				struct sdrsdramc_register *sdrsdramc_cfg);

#endif /* #ifndef __SDRSDRAMC_H__ */
