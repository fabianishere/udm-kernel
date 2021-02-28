/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_H__
#define __AL_ERR_EVENTS_H__

#include "al_mod_hal_common.h"

#define AL_ERR_EVENTS_IC_MAX		3

/*******************************************************************************
 ** enums
 ******************************************************************************/
/** error collection mode */
enum al_mod_err_events_collect {
	AL_ERR_EVENTS_COLLECT_DEFAULT, /**< Use Default error values */
	AL_ERR_EVENTS_COLLECT_POLLING, /**< polling mode */
	AL_ERR_EVENTS_COLLECT_INTERRUPT, /**< interrupt mode */
};

/** error severity */
enum al_mod_err_events_err_sev {
	AL_ERR_EVENTS_CORRECTABLE, /**< Correctable error */
	AL_ERR_EVENTS_UNCORRECTABLE, /**< Uncorrectable error */
};

/** error module primary ID */
enum al_mod_err_events_primary_module {
	AL_ERR_EVENTS_MODULE_NONE = 0,
	AL_ERR_EVENTS_MODULE_SSM, /**< SSM ID */
	AL_ERR_EVENTS_MODULE_ETH, /**< ETH ID */
	AL_ERR_EVENTS_MODULE_SATA, /**< SATA ID */
	AL_ERR_EVENTS_MODULE_PCIE, /**< PCIe ID */
	AL_ERR_EVENTS_MODULE_SYS_FABRIC, /**< System Fabric ID */
	AL_ERR_EVENTS_MODULE_PBS, /**< PBS ID */
	AL_ERR_EVENTS_MODULE_SERDES_25G, /**< SERDES 25G ID */
	AL_ERR_EVENTS_MODULE_SERDES_AVG, /**< AVG SERDES ID */
	AL_ERR_EVENTS_MODULE_L1, /**< L1 cache ID */
	AL_ERR_EVENTS_MODULE_L2, /**< L2 cache ID */
	AL_ERR_EVENTS_MODULE_NUM,
};

/** error module secondary ID */
enum al_mod_err_events_sec_module {
	AL_ERR_EVENTS_SEC_MODULE_NONE = 0,
	AL_ERR_EVENTS_SEC_MODULE_UDMA, /**< UDMA ID */
	AL_ERR_EVENTS_SEC_MODULE_UNIT_ADAPTER, /**< Unit adapter ID */
	AL_ERR_EVENTS_SEC_MODULE_SSM_CRYPTO, /**< SSM Crypto ID */
	AL_ERR_EVENTS_SEC_MODULE_SSM_RAID, /**< SSM RAID ID */
	AL_ERR_EVENTS_SEC_MODULE_SSM_CRC, /**< SSM CRC ID */
	AL_ERR_EVENTS_SEC_MODULE_ETH_EC, /**< ETH EC ID */
	AL_ERR_EVENTS_SEC_MODULE_ETH_EC_SEC, /**< ETH EC Secondary level ID */
	AL_ERR_EVENTS_SEC_MODULE_ETH_MAC, /**< ETH MAC ID */
	AL_ERR_EVENTS_SEC_MODULE_ETH_EPE, /**< ETH EPE */
	AL_ERR_EVENTS_SEC_MODULE_ETH_ALT_PROC_0, /**< ETH alternative processing 0 */
	AL_ERR_EVENTS_SEC_MODULE_ETH_ALT_PROC_1, /**< ETH alternative processing 1 */
	AL_ERR_EVENTS_SEC_MODULE_ETH_ALT_PROC_2, /**< ETH alternative processing 2 */
	AL_ERR_EVENTS_SEC_MODULE_PCIE_AXI, /**< PCIe AXI ID */
	AL_ERR_EVENTS_SEC_MODULE_PCIE_APP, /**< PCIe Application ID */
	AL_ERR_EVENTS_SEC_MODULE_PCIE_LOGGING, /**< PCIe logging ID */
	AL_ERR_EVENTS_SEC_MODULE_NUM,
};

/*******************************************************************************
 ** Structures
 ******************************************************************************/

struct al_mod_err_events_handle;
struct al_mod_err_events_module;
struct al_mod_err_events_field;

typedef int (*al_mod_err_events_print_cb)(const char *format, ...);
typedef void (*al_mod_err_events_error_cb)(struct al_mod_err_events_field *);

struct al_mod_err_events_field {
	/** Field name */
	const char *name;

	/** Field ID */
	unsigned int id;

	/** Field severity */
	enum al_mod_err_events_err_sev sev;

	/** Field Statistics counter */
	unsigned int counter;

	/** IOFIC ID */
	uint8_t iofic_id;

	/** IOFIC group ID */
	unsigned int iofic_group;

	/** IOFIC interrupt bit number */
	unsigned int iofic_bit;

	/** is field valid */
	al_mod_bool valid;

	/** collection mode */
	enum al_mod_err_events_collect collect_mode;

	/**
	 * print field Statistics
	 *
	 * @param field field object
	 * @param print_header print module header info
	 *
	 * @return none
	 */
	void (*print)(struct al_mod_err_events_field *, al_mod_bool);

	/**
	 * collect field errors
	 *
	 * @param field field object
	 * @param cause cause register value
	 *
	 * @return none
	 */
	void (*collect)(struct al_mod_err_events_field *, uint32_t);

	/**
	 * mask field error
	 *
	 * @param field field object
	 *
	 * @return none
	 */
	void (*mask)(struct al_mod_err_events_field *);

	/* Pointer to the field's parent err_events object */
	struct al_mod_err_events_module *parent_module;
};

struct al_mod_err_events_module {
	/** Module name */
	char name[32];

	/** Module primary ID */
	enum al_mod_err_events_primary_module primary_module;

	/** Module secondary ID */
	enum al_mod_err_events_sec_module secondary_module;

	/** Module Primary index ID */
	unsigned int primary_index;

	/** Module secondary index ID */
	unsigned int secondary_index;

	/** Pointer to statistics handle */
	struct al_mod_err_events_handle *handle;

	/** IC regs bases */
	void __iomem *ic_regs_bases[AL_ERR_EVENTS_IC_MAX];

	/** Number of ICs */
	unsigned int ic_size;

	/** Number of fields */
	unsigned int fields_size;

	/** Fields ARRAY pointer */
	struct al_mod_err_events_field *fields;

	/** Has polling mode fields */
	al_mod_bool has_polling_fields;

	/**
	 * Collect errors
	 *
	 * @param module module object
	 * @param collect collection mode
	 *
	 * @return 0 on success, errno otherwise
	 */
	int (*collect)(struct al_mod_err_events_module *,
		       enum al_mod_err_events_collect);

	/**
	 * Print statistics
	 *
	 * @param module module object
	 *
	 * @return none
	 */
	void (*print)(struct al_mod_err_events_module *);

	/**
	 * Clear statistics
	 *
	 * @param module module object
	 *
	 * @return none
	 */
	void (*clear)(struct al_mod_err_events_module *);

	/**
	 * Test errors collection
	 *
	 * @param module module object
	 *
	 * @return AL_TRUE on success, AL_FALSE otherwise
	 */
	al_mod_bool (*test)(struct al_mod_err_events_module *);

	/**
	 * Set cause for a field
	 *
	 * @param field field to set cause for
	 *
	 * @return none
	 */
	void (*set_cause)(struct al_mod_err_events_field *field);

	/**
	 * Get a field attributes
	 *
	 * @param field field pointer
	 * @param mask_status field is masked
	 * @param cause_status field cause is set
	 * @param abort_status field abort enabled
	 *
	 * @return none
	 */
	void (*get_attrs)(const struct al_mod_err_events_field *field,
		al_mod_bool *mask_status, al_mod_bool *cause_status, al_mod_bool *abort_status);

	/**
	 * Is this object has anything to collect.
	 * Note: set by init function.
	 */
	al_mod_bool enabled;

	/** module private data */
	void *private_data;

	/** Pointer to next Module */
	struct al_mod_err_events_module *next_comp;
};

struct al_mod_err_events_handle_init_params {
	/** PBS registers base address */
	void __iomem *pbs_regs_base;

	/** Print callback to be used in print functions */
	al_mod_err_events_print_cb print_cb;

	/** error callback to be used when error is found */
	al_mod_err_events_error_cb error_cb;

	/**
	 * error threshold
	 * if specific error counter is greater than this field,
	 * its interrupt will be masked.
	 * value of 0 means dont mask
	 */
	unsigned int error_threshold;
};

struct al_mod_err_events_handle {
	/** PBS register base address */
	void __iomem *pbs_regs_base;

	/** error print callback */
	al_mod_err_events_print_cb print_cb;

	/** error callback */
	al_mod_err_events_error_cb error_cb;

	/**
	 * error threshold
	 * if specific error counter is greater than this field,
	 * its interrupt will be masked.
	 * value of 0 means dont mask
	 */
	unsigned int error_threshold;

	/** device ID */
	unsigned int dev_id;

	/* List of components */
	struct al_mod_err_events_module *first_comp;
};

/*******************************************************************************
 ** Module API functions
 ******************************************************************************/
/*
 * Clear module statistics
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_module_clear(struct al_mod_err_events_module *module);

/*
 * Collect module errors on specific mode
 *
 * @param module initialized object
 * @param collect collection mode
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_module_collect(struct al_mod_err_events_module *module,
				 enum al_mod_err_events_collect collect);

/*
 * Print module statistics
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_module_print(struct al_mod_err_events_module *module);

/*
 * Run Module self test
 * Using this function requires the user to change
 * the handle error callback to not crash the system
 * as uncorrectable error interrupts will be generated.
 * the change should be before calling this function
 *
 * @param module initialized object
 *
 * @return AL_TRUE if test passed, AL_FALSE otherwise
 */
al_mod_bool al_mod_err_events_module_test(struct al_mod_err_events_module *module);

/*
 * Mask module interrupts
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_module_ints_mask(struct al_mod_err_events_module *module);

/*******************************************************************************
 ** Handle API functions
 ******************************************************************************/
/*
 * init error handle
 *
 * @param handle initialized object
 * @param init handle init parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_handle_init(struct al_mod_err_events_handle *handle,
			      struct al_mod_err_events_handle_init_params *init);

/*
 * Collect handle errors
 *
 * @param handle initialized object
 * @param collect collection mode
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_handle_collect(struct al_mod_err_events_handle *handle,
				 enum al_mod_err_events_collect collect);

/*
 * run self test on all modules in handle
 * Using this function requires the user to change
 * the handle error callback to not crash the system
 * as uncorrectable error interrupts will be generated.
 * the change should be before calling this function
 *
 * @param handle initialized object
 *
 * @return AL_TRUE if test passed, AL_FALSE otherwise
 */
al_mod_bool al_mod_err_events_handle_test(struct al_mod_err_events_handle *handle);

/*
 * clear handle statistics
 *
 * @param handle initialized object
 *
 * @return none
 */
void al_mod_err_events_handle_clear(struct al_mod_err_events_handle *handle);

/*
 * Print handle statistics
 *
 * @param handle initialized object
 *
 * @return none
 */
void al_mod_err_events_handle_print(struct al_mod_err_events_handle *handle);

/*
 * Mask handle interrupts
 *
 * @param handle initialized object
 *
 * @return none
 */
void al_mod_err_events_handle_ints_mask(struct al_mod_err_events_handle *handle);

#endif /* __AL_ERR_EVENTS_H__ */
