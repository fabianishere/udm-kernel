ifeq ($(CONFIG_ARCH_ALPINE),)
CONFIG_NET_AL_ETH=m

# Alpine V2 PCI device IDs
ALPINE_V2_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V2_10=0x1010
ALPINE_V2_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A=0x1020
ALPINE_V2_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A1=0x1021
ALPINE_V2_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V2_K2C=0x1022

ifeq ($(ALPINE_V2_NICS),1)
KBUILD_CPPFLAGS+=$(ALPINE_V2_PCI_DEVICE_IDS)
KBUILD_CPPFLAGS+=-DDRV_MODULE_NAME=\"al_eth_v2\"
AL_ETH_DRIVER_NAME:=al_eth_drv_v2
endif

# Alpine V3 PCI device IDs
ALPINE_V3_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V3_100=0x1030
ALPINE_V3_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V3_4x25=0x1024
ALPINE_V3_PCI_DEVICE_IDS+=-DPCI_DEVICE_ID_AL_ETH_NIC_V3_4x25_EX=0x1025

ifeq ($(ALPINE_V3_NICS),1)
KBUILD_CPPFLAGS+=$(ALPINE_V3_PCI_DEVICE_IDS)
KBUILD_CPPFLAGS+=-DDRV_MODULE_NAME=\"al_eth_v3\"
AL_ETH_DRIVER_NAME:=al_eth_drv_v3
endif

ifeq ($(ALPINE_ALL_NICS),1)
KBUILD_CPPFLAGS+=$(ALPINE_V2_PCI_DEVICE_IDS)
KBUILD_CPPFLAGS+=$(ALPINE_V3_PCI_DEVICE_IDS)
KBUILD_CPPFLAGS+=-DDRV_MODULE_NAME=\"al_eth\"
AL_ETH_DRIVER_NAME:=al_eth_drv
endif

else
AL_ETH_DRIVER_NAME:=al_eth_drv
endif

obj-$(CONFIG_NET_AL_ETH) += $(AL_ETH_DRIVER_NAME).o
$(AL_ETH_DRIVER_NAME)-objs := al_eth.o al_mod_eth_sysfs.o kcompat.o\
		   al_mod_hal_eth_main.o al_mod_hal_eth_kr.o al_mod_hal_eth_epe.o al_mod_hal_eth_common.o\
		   al_mod_hal_eth_mac.o al_mod_hal_eth_mac_internal.o al_mod_hal_eth_mac_v1_v2.o\
		   al_mod_hal_eth_mac_v3.o al_mod_hal_eth_mac_v4.o al_mod_hal_eth_rfw.o\
		   al_mod_hal_udma_main.o al_mod_hal_udma_debug.o al_mod_hal_udma_iofic.o\
		   al_mod_hal_udma_config.o\
		   al_mod_hal_serdes_hssp.o al_mod_hal_serdes_25g.o\
		   al_mod_hal_pbs_utils.o al_mod_hal_unit_adapter.o al_mod_hal_iofic.o\
		   al_mod_eth_lm_retimer_br210.o al_mod_eth_lm_retimer_ds25.o al_mod_eth_lm_retimer_br410.o\
		   al_mod_eth_lm_retimer.o al_mod_eth_v4_lm.o al_mod_init_eth_lm.o al_mod_init_eth_kr.o\
		   al_mod_serdes.o\
		   al_mod_err_events.o al_mod_err_events_common.o al_mod_err_events_eth.o\
		   al_mod_err_events_eth_epe.o al_mod_err_events_io_fabric.o al_mod_err_events_udma.o

ifeq ($(CONFIG_ARCH_ALPINE),)
$(AL_ETH_DRIVER_NAME)-objs += al_mod_hal_i2c.o al_mod_hal_udma_config_ex.o\
		   al_mod_hal_pcie_sriov_emul.o al_mod_hal_pcie_ex.o al_mod_hal_pcie.o\
		   al_mod_hal_pcie_reg_ptr_set_rev1.o al_mod_hal_pcie_reg_ptr_set_rev2.o\
		   al_mod_hal_pcie_reg_ptr_set_rev3.o al_mod_hal_pcie_reg_ptr_set_rev4.o\
		   al_mod_hal_pcie_reg_ptr_set_rev5.o\
		   al_mod_hal_pcie_ex_reg_ptr_set_rev1.o al_mod_hal_pcie_ex_reg_ptr_set_rev2.o\
		   al_mod_hal_pcie_ex_reg_ptr_set_rev3.o al_mod_hal_pcie_ex_reg_ptr_set_rev4.o\
		   al_mod_hal_pcie_ex_reg_ptr_set_rev5.o\
		   al_mod_hal_iomap_dynamic.o\
		   platform/alpine_v1/src/iomap_dynamic/al_mod_hal_iomap_dynamic.o\
		   platform/alpine_v2/src/iomap_dynamic/al_mod_hal_iomap_dynamic.o\
		   platform/alpine_v3/src/iomap_dynamic/al_mod_hal_iomap_dynamic.o\
		   al_mod_tam.o
else
$(AL_ETH_DRIVER_NAME)-objs += al_mod_eth_common.o alpine_serdes.o alpine_avg_serdes.o alpine_group_lm.o\
		   al_mod_hal_eth_field.o\
		   al_mod_hal_serdes_avg_init.o al_mod_hal_serdes_avg_utils.o al_mod_hal_serdes_avg.o\
		   al_mod_hal_sbus.o\
		   al_mod_eth_group_lm.o\
		   al_mod_hal_live_update.o al_mod_eth_v3_lm_live_update.o
endif

ifeq ($(CONFIG_ARCH_ALPINE),)

ifeq ($(EMUL),1)
KBUILD_CPPFLAGS+=-DEMUL_PLAT=1
endif

ifneq ($(KBUILD_EXTMOD),)
# The equivalent of -I. , using kernel's variable derived from M= flag
KBUILD_CPPFLAGS+=-I$(KBUILD_EXTMOD)
endif

KERNEL_VERSION ?= $(shell uname -r)

KBUILD_CPPFLAGS+=-DCONFIG_AL_ETH_ALLOC_PAGE
#KBUILD_CPPFLAGS+=-DCONFIG_AL_ETH_ERR_EVENTS_TEST

# AL PCI vendor ID
KBUILD_CPPFLAGS+=-DPCI_VENDOR_ID_ANNAPURNA_LABS=0x1c36

KBUILD_CPPFLAGS+=-DDRV_MODULE_VERSION=\"3.5.3\"

KBUILD_CPPFLAGS+=-DAL_DEV_ID_ALPINE_V1=0 \
        -DAL_DEV_ID_ALPINE_V2=1 \
        -DAL_DEV_ID_ALPINE_V3=2 \
        -DAL_DEV_ID_ALPINE_V4=3 \

alpine_v3:
	make -C /lib/modules/$(KERNEL_VERSION)/build M=$(CURDIR) modules ALPINE_V3_NICS=1

alpine_v2:
	make -C /lib/modules/$(KERNEL_VERSION)/build M=$(CURDIR) modules ALPINE_V2_NICS=1

all:
	make -C /lib/modules/$(KERNEL_VERSION)/build M=$(CURDIR) modules ALPINE_ALL_NICS=1

clean:
	make -C /lib/modules/$(KERNEL_VERSION)/build M=$(CURDIR) clean

else

KBUILD_CFLAGS_KERNEL += -I$(srctree)/drivers/net/ethernet/al/

endif # CONFIG_ARCH_ALPINE
