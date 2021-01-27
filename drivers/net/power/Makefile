include $(srctree)/drivers/soc/alpine/alpine_hal.mk

KBUILD_CFLAGS_KERNEL += -I$(srctree)/include/uapi/linux -I$(srctree)/include/uapi/linux/can

obj-$(CONFIG_UBNT_POE_DEV)  += ubnt_poe_dev.o
obj-$(CONFIG_UBNT_POE_S2P)  += ubnt_poe_s2p.o
obj-$(CONFIG_POE_BCM59111)  += bcm59111.o

