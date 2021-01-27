#ifndef __DRAM_MARGINS_AGENT_H__
#define __DRAM_MARGINS_AGENT_H__

#include "al_hal_ddr.h"

/** Working area size required for calculating the DRAM margins */
#define AL_DRAM_MARGINS_AGENT_WORK_AREA_SIZE		(1 << 19)

/** Working area alignment required for calculating the DRAM margins */
#define AL_DRAM_MARGINS_AGENT_WORK_AREA_ALIGNED_BITS	19

/** DRAM margins operation parameters */
struct al_dram_margins_params {
	/**
	 * Working area required for calculating the DRAM margins
	 * Physical address
	 * Size:		AL_DRAM_MARGINS_AGENT_WORK_AREA_SIZE
	 * Alignment:	AL_DRAM_MARGINS_AGENT_WORK_AREA_ALIGNED_BITS
	 */
	al_phys_addr_t			work_area;
	/**
	 * An indication of first margins calculation in a series of calculations
	 * Running a series of calculations without using the first/last mechanism will result
	 * in DQS/DQ drift over time
	 */
	al_bool				first;
	/**
	 * An indication of last margins calculation in a series of calculations
	 * Running a series of calculations without using the first/last mechanism will result
	 * in DQS/DQ drift over time
	 */
	al_bool				last;
	/**
	 * An indication for using the walking 0 pattern as part of the calculation
	 * Suggested value: AL_FALSE
	 */
	al_bool				pattern_walk_0;
	/**
	 * An indication for using the walking 1 pattern as part of the calculation
	 * Suggested value: AL_FALSE
	 */
	al_bool				pattern_walk_1;
	/**
	 * An indication for using LFSR pattern as part of the calculation
	 * Suggested value: AL_TRUE
	 */
	al_bool				pattern_lfsr;
	/**
	 * An indication for using a user pattern as part of the calculation
	 * Suggested value: AL_FALSE
	 */
	al_bool				pattern_user;
	/** User pattern even part (16 bits) */
	uint16_t			pattern_user_even;
	/** User pattern odd part (16 bits) */
	uint16_t			pattern_user_odd;
	/**
	 * The number of iterations
	 * Suggested value: 1
	 */
	unsigned int			iterations_per_sample;
	/** The DRAM margins result */
	struct al_dram_margins_result	*margins_result;
};

/** DRAM margins result per bit */
struct al_dram_margins_result_per_bit {
	int8_t rd_dqs;	/** Read DQS */
	int8_t rd_dq;	/** Read DQ */

	int8_t rd_min;	/** Read minimal working delay */
	int8_t rd_curr;	/** Read current delay (rd_dq - rd_dqs) */
	int8_t rd_max;	/** Read maximal working delay */

	int8_t wr_dqs;	/** Write DQS */
	int8_t wr_dq;	/** Write DQ */

	int8_t wr_min;	/** Write minimal working delay */
	int8_t wr_curr;	/** Write current delay (rd_dq - rd_dqs) */
	int8_t wr_max;	/** Write maximal working delay */
};

/** DRAM margins result per lane */
struct al_dram_margins_result_per_lane {
	struct al_dram_margins_result_per_bit bits[AL_DDR_BIST_DQ_BITS];
};

/** DRAM margins result per rank */
struct al_dram_margins_result_per_rank {
	struct al_dram_margins_result_per_lane lanes[AL_DDR_PHY_NUM_BYTE_LANES];
};

/** DRAM margins result */
struct al_dram_margins_result {
	/** Detected rank mask */
	unsigned int				rank_mask;
	/** Active lanes detected */
	int					active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];
	/** Ranks margins result */
	struct al_dram_margins_result_per_rank	ranks[AL_DDR_NUM_RANKS];
};

/**
 * DRAM margins calculation (with param struct)
 *
 * @param	dram_ch
 *		DRAM channel
 * @param	params
 *		Operation parameters
 *
 * @returns	0 upon success
 */
#define AL_DRAM_MARGINS_HAS_EX
void al_dram_margins_ex(
	unsigned int				dram_ch,
	const struct al_dram_margins_params	*params);

/**
 * DRAM margins calculation
 *
 * @param	work_area
 *		Working area required for calculating the DRAM margins
 *		Physical address
 *		Size:		AL_DRAM_MARGINS_AGENT_WORK_AREA_SIZE
 *		Alignment:	AL_DRAM_MARGINS_AGENT_WORK_AREA_ALIGNED_BITS
 * @param	first
 *		An indication of first margins calculation in a series of calculations
 *		Running a series of calculations without using the first/last mechanism will result
 *		in DQS/DQ drift over time
 * @param	last
 *		An indication of last margins calculation in a series of calculations
 *		Running a series of calculations without using the first/last mechanism will result
 *		in DQS/DQ drift over time
 * @param	pattern_walk_0
 *		An indication for using the walking 0 pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_walk_1
 *		An indication for using the walking 1 pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_lfsr
 *		An indication for using LFSR pattern as part of the calculation
 *		Suggested value: AL_TRUE
 * @param	pattern_user
 *		An indication for using a user pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_user_even
 *		User pattern even part (16 bits)
 * @param	pattern_user_odd
 *		User pattern odd part (16 bits)
 * @param	iterations_per_sample
 *		The number of iterations
 *		Suggested value: 1
 * @param	margins_result
 *		The DRAM margins result
 *
 * @returns	0 upon success
 */
void al_dram_margins(
	al_phys_addr_t			work_area,
	al_bool				first,
	al_bool				last,
	al_bool				pattern_walk_0,
	al_bool				pattern_walk_1,
	al_bool				pattern_lfsr,
	al_bool				pattern_user,
	uint16_t			pattern_user_even,
	uint16_t			pattern_user_odd,
	unsigned int			iterations_per_sample,
	struct al_dram_margins_result	*margins_result);

/**
 * DRAM margins agent init
 *
 * @returns	0 upon success
 */
int al_dram_margins_agent_init(void);

/**
 * DRAM margins agent run
 *
 * @param	work_area
 *		Working area required for calculating the DRAM margins
 *		Physical address
 *		Size: DRAM_MARGINS_AGENT_WORK_AREA_SIZE
 * @param	first
 *		An indication of first margins calculation in a series of calculations
 *		Running a series of calculations without using the first/last mechanism will result
 *		in DQS/DQ drift over time
 * @param	last
 *		An indication of last margins calculation in a series of calculations
 *		Running a series of calculations without using the first/last mechanism will result
 *		in DQS/DQ drift over time
 * @param	pattern_walk_0
 *		An indication for using the walking 0 pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_walk_1
 *		An indication for using the walking 1 pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_lfsr
 *		An indication for using LFSR pattern as part of the calculation
 *		Suggested value: AL_TRUE
 * @param	pattern_user
 *		An indication for using a user pattern as part of the calculation
 *		Suggested value: AL_FALSE
 * @param	pattern_user_even
 *		User pattern even part (16 bits)
 * @param	pattern_user_odd
 *		User pattern odd part (16 bits)
 * @param	iterations_per_sample
 *		The number of iterations
 *		Suggested value: 1
 * @param	result
 *		The DRAM margins result
 *
 * @returns	0 upon success
 */
int al_dram_margins_agent_run(
	al_phys_addr_t			work_area,
	al_bool				first,
	al_bool				last,
	al_bool				pattern_walk_0,
	al_bool				pattern_walk_1,
	al_bool				pattern_lfsr,
	al_bool				pattern_user,
	uint16_t			pattern_user_even,
	uint16_t			pattern_user_odd,
	unsigned int			iterations_per_sample,
	struct al_dram_margins_result	*result);

/**
 * DRAM margins agent run (with param struct)
 *
 * @param	dram_ch
 *		DRAM channel
 * @param	params
 *		Operation parameters
 *
 * @returns	0 upon success
 */
int al_dram_margins_agent_run_ex(
	unsigned int				dram_ch,
	const struct al_dram_margins_params	*params);
/**
 * DRAM margins result printout
 *
 * @param	results
 *		The DRAM margins result
 * @param	width
 *		Printout width
 *		If less than 40, means how many taps per row
 */
void al_dram_margins_result_print(
	struct al_dram_margins_result	*results,
	unsigned int			width);

/**
 * DRAM margins result .csv printout
 *
 * @param	results
 *		The DRAM margins result
 */
void al_dram_margins_result_print_csv(
	struct al_dram_margins_result	*results);

#endif

