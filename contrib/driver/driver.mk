#
# Makefile for the contributed drivers
#
CONTRIB_DRIVERS_SRC:=$(TOPDIR)/contrib/driver

#COBJS-$(CONFIG_ABC)	+= $(CONTRIB_DRIVERS_SRC)/adc.o
COBJS-$(CONFIG_SDDRC)	+= $(CONTRIB_DRIVERS_SRC)/sdrsdramc.o
