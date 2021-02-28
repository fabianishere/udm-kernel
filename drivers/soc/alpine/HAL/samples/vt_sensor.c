#include "samples.h"
#include "al_hal_iomap.h"
#include "al_hal_vt_sensor.h"

#ifndef AL_SRD_CMPLX_HS_SBUS_MASTER_BASE
#define  AL_SRD_CMPLX_HS_SBUS_MASTER_BASE	AL_PBS_SBUS_MASTER_BASE
#endif

#ifndef AL_SBUS_ADDR_THERMAL_SENSOR
#define  AL_SBUS_ADDR_THERMAL_SENSOR 7
#endif

static void vt_sensor_sample(enum al_vt_sensor_type sensor_type, al_bool loop_sampling)
{
	static struct al_sbus_obj sbus_obj;
	struct al_vt_sensor_handle handle;
	struct al_vt_sensor_handle_init_params handle_init_params = {
			.pbs_regs_base = (void __iomem *)(uintptr_t)AL_PBS_REGFILE_BASE,
			.thermal_sensor_reg_base = (void __iomem *)(uintptr_t)AL_TEMP_SENSOR_BASE,
			.sbus_obj = &sbus_obj,
			.sbus_slave_addr = AL_SBUS_ADDR_THERMAL_SENSOR,
			.sensor_type = sensor_type
		};

	al_sbus_handle_init(&sbus_obj, (void __iomem *)(uintptr_t)AL_SRD_CMPLX_HS_SBUS_MASTER_BASE);

	al_vt_sensor_handle_init(&handle, &handle_init_params);
	al_vt_sensor_enable_set(&handle, 1);

	while (!al_vt_sensor_is_ready(&handle))
		al_udelay(1);

	al_vt_sensor_trigger_continuous(&handle);

	do {
		while (!al_vt_sensor_readout_is_valid(&handle))
			al_udelay(1);

		if (handle.sensor_type == al_vt_sensor_type_core_vdd)
			al_print("voltage: %d mV\n", al_vt_sensor_readout_get(&handle));
		else if (handle.sensor_type == al_vt_sensor_type_thermal)
			al_print("temperature: %d celsius\n", al_vt_sensor_readout_get(&handle));

		al_msleep(1000);
	} while (loop_sampling);

	al_vt_sensor_enable_set(&handle, 0);

}

int main(void)
{
	al_info("start of voltage_sensor_sample!\n");

	/* example for voltage sensor sampling */
	vt_sensor_sample(al_vt_sensor_type_core_vdd, AL_TRUE);

	return 0;

}
