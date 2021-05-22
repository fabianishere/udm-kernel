#include "al_hal_unit_adapter.h"
#include "al_hal_iomap.h"

#ifndef AL_SATA_ADAPTER_BASE
#define AL_SATA_ADAPTER_BASE(idx)	0
#endif

static int unit_adapter_pd(void)
{
	int err;
	struct al_unit_adapter sata_adapter;

	err = al_unit_adapter_handle_init(&sata_adapter, AL_UNIT_ADAPTER_TYPE_SATA,
		(void __iomem *)AL_SATA_ADAPTER_BASE(0), NULL, NULL, NULL, NULL);
	if (err)
		return err;

	al_unit_adapter_mem_shutdown_ctrl(&sata_adapter, AL_TRUE);
	al_unit_adapter_clk_gating_ctrl(&sata_adapter, AL_TRUE);
	al_unit_adapter_pd_ctrl(&sata_adapter, AL_TRUE);

	return 0;
}

int main(void)
{
	unit_adapter_pd();

	return 0;
}

