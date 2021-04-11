#
# Makefile for the contributed drivers
#
CONTRIB_DRIVERS_SRC:=contrib/driver

#COBJS-$(CONFIG_ABC)		+= $(CONTRIB_DRIVERS_SRC)/adc.o
COBJS-$(CONFIG_SDRSDRAM)	+= $(CONTRIB_DRIVERS_SRC)/sdrsdramc.o
