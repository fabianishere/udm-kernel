#include "samples.h"
#include "al_hal_thermal_sensor.h"
#include "al_hal_iomap.h"

int thermal_sensor_sample(void)
{
	struct al_thermal_sensor_handle handle;
	int err;

	err = al_thermal_sensor_handle_init(&handle, (void __iomem *)AL_TEMP_SENSOR_BASE,
		(void __iomem *)AL_PBS_REGFILE_BASE);
	if (err)
		return err;

	al_thermal_sensor_enable_set(&handle, 1);

	while (!al_thermal_sensor_is_ready(&handle))
		al_udelay(1);

	al_thermal_sensor_trigger_continuous(&handle);

	while (1) {
		while (!al_thermal_sensor_readout_is_valid(&handle))
			al_udelay(1);

		al_print("temprature: %d degrees\n", al_thermal_sensor_readout_get(&handle));

		al_msleep(1000);
	}

	al_thermal_sensor_enable_set(&handle, 0);

	return 0;
}

