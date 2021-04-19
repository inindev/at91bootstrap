# Makefile for AT91-Bootstrap drivers directory
# DIRS				+= drivers

DRIVERS:=
DRIVERS_SRC:=$(TOPDIR)/driver

COBJS-$(CONFIG_DEBUG)		+= $(DRIVERS_SRC)/debug.o

COBJS-$(CONFIG_SCLK)		+= $(DRIVERS_SRC)/at91_slowclk.o

COBJS-y				+= $(DRIVERS_SRC)/common.o
COBJS-y				+= $(DRIVERS_SRC)/at91_pio.o
COBJS-y				+= $(DRIVERS_SRC)/pmc.o
COBJS-y				+= $(DRIVERS_SRC)/at91_pit.o
COBJS-y				+= $(DRIVERS_SRC)/at91_wdt.o
COBJS-y				+= $(DRIVERS_SRC)/at91_usart.o
COBJS-y				+= $(DRIVERS_SRC)/at91_rstc.o

COBJS-$(CPU_HAS_L2CC)		+= $(DRIVERS_SRC)/lp310_l2cc.o

COBJS-$(CONFIG_SDRAM)		+= $(DRIVERS_SRC)/sdramc.o
#COBJS-$(CONFIG_SDDRC)		+= $(DRIVERS_SRC)/sddrc.o
COBJS-$(CONFIG_DDRC)		+= $(DRIVERS_SRC)/ddramc.o

COBJS-$(CONFIG_AT91_MCI)	+= $(DRIVERS_SRC)/at91_mci.o
COBJS-$(CONFIG_SDHC)		+= $(DRIVERS_SRC)/sdhc.o

COBJS-$(CONFIG_SDCARD)		+= $(DRIVERS_SRC)/mci_media.o
COBJS-$(CONFIG_SDCARD)		+= $(DRIVERS_SRC)/sdcard.o

COBJS-$(CONFIG_NANDFLASH)	+= $(DRIVERS_SRC)/nandflash.o
COBJS-$(CONFIG_USE_PMECC)	+= $(DRIVERS_SRC)/pmecc.o
COBJS-$(CONFIG_ENABLE_SW_ECC) 	+= $(DRIVERS_SRC)/hamming.o

COBJS-$(CONFIG_SPI_FLASH)	+= $(DRIVERS_SRC)/spi_flash/spi_flash.o
COBJS-$(CONFIG_SPI_FLASH)	+= $(DRIVERS_SRC)/spi_flash/sfdp.o
COBJS-$(CONFIG_SPI_FLASH)	+= $(DRIVERS_SRC)/spi_flash/spi_nor.o
COBJS-$(CONFIG_SPI_FLASH)	+= $(DRIVERS_SRC)/spi_flash/spi_nor_ids.o

COBJS-$(CONFIG_SPI)		+= $(DRIVERS_SRC)/at91_spi.o
COBJS-$(CONFIG_SPI)		+= $(DRIVERS_SRC)/spi_flash.o
COBJS-$(CONFIG_QSPI)		+= $(DRIVERS_SRC)/at91_qspi.o
COBJS-$(CONFIG_DATAFLASH)	+= $(DRIVERS_SRC)/dataflash.o

COBJS-$(CONFIG_FLASH)		+= $(DRIVERS_SRC)/flash.o

COBJS-$(CONFIG_LOAD_LINUX)	+= $(DRIVERS_SRC)/load_kernel.o
COBJS-$(CONFIG_LOAD_ANDROID)	+= $(DRIVERS_SRC)/load_kernel.o

COBJS-$(CONFIG_LOAD_ONE_WIRE)	+= $(DRIVERS_SRC)/ds24xx.o
COBJS-$(CONFIG_LOAD_EEPROM)	+= $(DRIVERS_SRC)/at24xx.o
COBJS-$(CONFIG_LOAD_HW_INFO)	+= $(DRIVERS_SRC)/board_hw_info.o

COBJS-$(CONFIG_MATRIX)		+= $(DRIVERS_SRC)/matrix.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/tz_utils.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/svc_handler.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/svc_mgr.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/monitor/mon_init.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/monitor/mon_switch.o
COBJS-$(CONFIG_ENTER_NWD)	+= $(DRIVERS_SRC)/monitor/mon_vectors.o

COBJS-$(CONFIG_PM)	+= $(DRIVERS_SRC)/pm.o
COBJS-$(CONFIG_TWI)	+= $(DRIVERS_SRC)/at91_twi.o
COBJS-$(CONFIG_ACT8865)	+= $(DRIVERS_SRC)/act8865.o
COBJS-$(CONFIG_MACB)	+= $(DRIVERS_SRC)/macb.o
COBJS-$(CONFIG_HDMI)	+= $(DRIVERS_SRC)/hdmi_SiI9022.o
COBJS-$(CONFIG_WM8904)	+= $(DRIVERS_SRC)/wm8904.o

COBJS-$(CONFIG_AES)		+= $(DRIVERS_SRC)/at91_aes.o
COBJS-$(CONFIG_SECURE)		+= $(DRIVERS_SRC)/secure.o

COBJS-$(CONFIG_BACKUP_MODE)	+= $(DRIVERS_SRC)/backup.o

COBJS-$(CONFIG_REDIRECT_ALL_INTS_AIC)	+= $(DRIVERS_SRC)/at91_aicredir.o
