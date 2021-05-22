include $(srctree)/drivers/soc/alpine/alpine_hal.mk
AL_DMA_DRIVER_NAME:=al_dma_drv
obj-$(CONFIG_AL_DMA) += $(AL_DMA_DRIVER_NAME).o
$(AL_DMA_DRIVER_NAME)-objs := al_dma.o \
			al_dma_core.o \
			al_dma_cleanup.o \
			al_dma_sysfs.o \
			al_dma_prep_interrupt.o \
			al_dma_prep_memcpy.o \
			al_dma_prep_memset.o \
			al_dma_prep_xor.o \
			al_dma_prep_xor_val.o \
			al_dma_prep_pq.o \
			al_dma_prep_pq_val.o
