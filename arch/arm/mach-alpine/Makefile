KBUILD_CFLAGS_KERNEL += -I$(srctree)/include/soc/alpine/
KBUILD_CFLAGS_KERNEL += -DAL_DEV_ID_ALPINE_V1=0 \
			-DAL_DEV_ID_ALPINE_V2=1 \
			-DAL_DEV_ID=AL_DEV_ID_ALPINE_V1 \
			-DCONFIG_GIC500_ARE=0
obj-$(CONFIG_ARCH_ALPINE)	+= alpine_machine.o
obj-$(CONFIG_ARCH_ALPINE)	+= al_init_sys_fabric.o
obj-$(CONFIG_SMP)		+= platsmp.o alpine_cpu_pm.o
