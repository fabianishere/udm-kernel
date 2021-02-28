#ifndef __HAL_SAMPLE_H__
#define __HAL_SAMPLE_H__

#include "al_hal_plat_types.h"

/**
 * Check whether register address is present in dump file
 *
 * @param	reg_addr
 *		The address of the register to look for
 *
 * @return	AL_TRUE if register is found in the dump file,
 *		AL_FALSE otherwise
 */
al_bool al_reg_dump_reg_is_in_dump(uintptr_t reg_addr);

/**
 * @brief   Initialize file descriptor of the dump file
 *
 * @param   dump_file_path path to register dump file
 * @return  0 if no error found
 */
int al_reg_dump_fd_init(const char *dump_file_path);

#endif
