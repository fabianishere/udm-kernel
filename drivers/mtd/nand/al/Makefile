include $(srctree)/drivers/soc/alpine/alpine_hal.mk

# TODO: temp solution to allow include raid.h - will be removed after
# implementing read/write with dma support
KBUILD_CPPFLAGS += -I$(srctree)/drivers/dma/al


obj-$(CONFIG_MTD_NAND_AL) += al-nand.o
al-nand-objs += al_nand.o
