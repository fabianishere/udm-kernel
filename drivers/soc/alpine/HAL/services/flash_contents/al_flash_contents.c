#include "al_hal_plat_types.h"
#include "al_hal_plat_services.h"
#include "al_hal_reg_utils.h"
#include "al_flash_contents.h"
#include "al_hal_iomap.h"

#define DEV_READ(buff, size)					\
	do {							\
		err = dev_read_func(offset_on_dev, buff, size);	\
		if (err)					\
			goto read_error;			\
		offset_on_dev += size;				\
	} while (0);

#define FORCED_INSTANCE_NUM_MARK				AL_BIT(0)
#define FORCED_INSTANCE_NUM_NUM_MASK				0x0000ff00
#define FORCED_INSTANCE_NUM_NUM_SHIFT				8
#define FORCED_INSTANCE_NUM_FALLBACK_NUM_MASK			0x00ff0000
#define FORCED_INSTANCE_NUM_FALLBACK_NUM_SHIFT			16
#define STAGE2_SIGNATURE_AND_MODULUS_SIZE			512

/*******************************************************************************
 ******************************************************************************/
static uint16_t chksum16(void *buff, int len)
{
	uint8_t *buff_ptr = (uint8_t *)buff;
	uint16_t val = 0;

	for (; len; len--, buff_ptr++)
		val += *buff_ptr;

	return val;
}

/*******************************************************************************
 ******************************************************************************/
static uint32_t chksum32(void *buff, int len)
{
	uint8_t *buff_ptr = (uint8_t *)buff;
	uint32_t val = 0;

	for (; len; len--, buff_ptr++)
		val += *buff_ptr;

	return val;
}

/*******************************************************************************
 ******************************************************************************/
static const char *_al_flash_obj_id_to_str(
	unsigned int id)
{
	switch (id) {
	case AL_FLASH_OBJ_ID_BOOT_MODE:		return "BOOT_MODE";
	case AL_FLASH_OBJ_ID_STG2:		return "STG2";
	case AL_FLASH_OBJ_ID_DT:		return "DT";
	case AL_FLASH_OBJ_ID_STG2_5:		return "STG2_5";
	case AL_FLASH_OBJ_ID_STG3:		return "STG3";
	case AL_FLASH_OBJ_ID_UBOOT:		return "UBOOT";
	case AL_FLASH_OBJ_ID_UBOOT_SCRIPT:	return "UBOOT_SCRIPT";
	case AL_FLASH_OBJ_ID_UBOOT_ENV:		return "UBOOT_ENV";
	case AL_FLASH_OBJ_ID_UBOOT_ENV_RED:	return "UBOOT_ENV_RED";
	case AL_FLASH_OBJ_ID_KERNEL:		return "KERNEL";
	case AL_FLASH_OBJ_ID_ROOT_FS:		return "ROOT_FS";
	case AL_FLASH_OBJ_ID_2ND_TOC:		return "2ND_TOC";
	case AL_FLASH_OBJ_ID_PRE_BOOT:		return "PRE_BOOT";
	case AL_FLASH_OBJ_ID_PRE_BOOT_V2:	return "PRE_BOOT_V2";
	case AL_FLASH_OBJ_ID_HW_PLD:		return "HW_PLD";
	case AL_FLASH_OBJ_ID_APCEA:		return "APCEA";
	case AL_FLASH_OBJ_ID_APCEA_CFG:		return "APCEA_CFG";
	case AL_FLASH_OBJ_ID_IPXE:		return "IPXE";
	case AL_FLASH_OBJ_ID_BOOT_APP:		return "BOOT_APP";
	case AL_FLASH_OBJ_ID_BOOT_APP_CFG:	return "BOOT_APP_CFG";
	case AL_FLASH_OBJ_ID_APP_0:		return "APP_0";
	case AL_FLASH_OBJ_ID_APP_0_CFG:		return "APP_0_CFG";
	case AL_FLASH_OBJ_ID_APP_1:		return "APP_1";
	case AL_FLASH_OBJ_ID_APP_1_CFG:		return "APP_1_CFG";
	case AL_FLASH_OBJ_ID_APP_2:		return "APP_2";
	case AL_FLASH_OBJ_ID_APP_2_CFG:		return "APP_2_CFG";
	case AL_FLASH_OBJ_ID_APP_3:		return "APP_3";
	case AL_FLASH_OBJ_ID_APP_3_CFG:		return "APP_3_CFG";
	case AL_FLASH_OBJ_ID_CRASH_DUMP:	return "CRASH_DUMP";
	case AL_FLASH_OBJ_ID_VPD:		return "VPD";
	case AL_FLASH_OBJ_ID_SERDES_25G_FW:	return "SERDES_25G_FW";
	case AL_FLASH_OBJ_ID_SERDES_AVG_PCIE_FW:	return "SERDES_AVG_PCIE_FW";
	case AL_FLASH_OBJ_ID_SERDES_AVG_NON_PCIE_FW:	return "SERDES_AVG_NON_PCIE_FW";
	case AL_FLASH_OBJ_ID_AVG_SBUS_MASTER_FW:	return "AVG_SBUS_MASTER_FW";
	case AL_FLASH_OBJ_ID_PUK0:		return "PUK0";
	case AL_FLASH_OBJ_ID_PUK1:		return "PUK1";
	case AL_FLASH_OBJ_ID_PUK2:		return "PUK2";
	case AL_FLASH_OBJ_ID_ATF_BL31:		return "ATF_BL31";
	case AL_FLASH_OBJ_ID_ATF_BL32:		return "ATF_BL32";
	case AL_FLASH_OBJ_ID_EEPROM:		return "EEPROM";
	case AL_FLASH_OBJ_ID_SPIS_PLD:		return "SPIS_PLD";
	}

	return "N/A";
}

/*******************************************************************************
 ******************************************************************************/
#define SIG_STR_MAX_LEN		40

static const char sig_str_hdr[] = "SIG_";

const char *al_flash_obj_id_to_str(
	unsigned int id)
{
	static char sig_str[SIG_STR_MAX_LEN];
	unsigned int i;
	unsigned int j;
	const char *str = _al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_REMOVE_SIG(id));

	if (!AL_FLASH_OBJ_ID_IS_SIG(id))
		return str;

	for (i = 0; sig_str_hdr[i]; i++) {
		al_assert(i < SIG_STR_MAX_LEN);
		sig_str[i] = sig_str_hdr[i];
	}
	for (j = 0; str[j]; j++) {
		al_assert((i + j) < SIG_STR_MAX_LEN);
		sig_str[i + j] = str[j];
	}
	al_assert((i + j) < SIG_STR_MAX_LEN);
	sig_str[i + j] = 0;

	return sig_str;
}

/*******************************************************************************
 ******************************************************************************/
static int _al_flash_obj_id_from_str(
	const char *str)
{
	if (!al_strcmp(str, "BOOT_MODE"))
		return AL_FLASH_OBJ_ID_BOOT_MODE;
	else if (!al_strcmp(str, "STG2"))
		return AL_FLASH_OBJ_ID_STG2;
	else if (!al_strcmp(str, "DT"))
		return AL_FLASH_OBJ_ID_DT;
	else if (!al_strcmp(str, "STG2_5"))
		return AL_FLASH_OBJ_ID_STG2_5;
	else if (!al_strcmp(str, "STG3"))
		return AL_FLASH_OBJ_ID_STG3;
	else if (!al_strcmp(str, "UBOOT"))
		return AL_FLASH_OBJ_ID_UBOOT;
	else if (!al_strcmp(str, "UBOOT_SCRIPT"))
		return AL_FLASH_OBJ_ID_UBOOT_SCRIPT;
	else if (!al_strcmp(str, "UBOOT_ENV"))
		return AL_FLASH_OBJ_ID_UBOOT_ENV;
	else if (!al_strcmp(str, "UBOOT_ENV_RED"))
		return AL_FLASH_OBJ_ID_UBOOT_ENV_RED;
	else if (!al_strcmp(str, "KERNEL"))
		return AL_FLASH_OBJ_ID_KERNEL;
	else if (!al_strcmp(str, "ROOT_FS"))
		return AL_FLASH_OBJ_ID_ROOT_FS;
	else if (!al_strcmp(str, "PRE_BOOT"))
		return AL_FLASH_OBJ_ID_PRE_BOOT;
	else if (!al_strcmp(str, "PRE_BOOT_V2"))
		return AL_FLASH_OBJ_ID_PRE_BOOT_V2;
	else if (!al_strcmp(str, "HW_PLD"))
		return AL_FLASH_OBJ_ID_HW_PLD;
	else if (!al_strcmp(str, "2ND_TOC"))
		return AL_FLASH_OBJ_ID_2ND_TOC;
	else if (!al_strcmp(str, "APCEA"))
		return AL_FLASH_OBJ_ID_APCEA;
	else if (!al_strcmp(str, "APCEA_CFG"))
		return AL_FLASH_OBJ_ID_APCEA_CFG;
	else if (!al_strcmp(str, "IPXE"))
		return AL_FLASH_OBJ_ID_IPXE;
	else if (!al_strcmp(str, "BOOT_APP"))
		return AL_FLASH_OBJ_ID_BOOT_APP;
	else if (!al_strcmp(str, "BOOT_APP_CFG"))
		return AL_FLASH_OBJ_ID_BOOT_APP_CFG;
	else if (!al_strcmp(str, "APP_0"))
		return AL_FLASH_OBJ_ID_APP_0;
	else if (!al_strcmp(str, "APP_0_CFG"))
		return AL_FLASH_OBJ_ID_APP_0_CFG;
	else if (!al_strcmp(str, "APP_1"))
		return AL_FLASH_OBJ_ID_APP_1;
	else if (!al_strcmp(str, "APP_1_CFG"))
		return AL_FLASH_OBJ_ID_APP_1_CFG;
	else if (!al_strcmp(str, "APP_2"))
		return AL_FLASH_OBJ_ID_APP_2;
	else if (!al_strcmp(str, "APP_2_CFG"))
		return AL_FLASH_OBJ_ID_APP_2_CFG;
	else if (!al_strcmp(str, "APP_3"))
		return AL_FLASH_OBJ_ID_APP_3;
	else if (!al_strcmp(str, "APP_3_CFG"))
		return AL_FLASH_OBJ_ID_APP_3_CFG;
	else if (!al_strcmp(str, "CRASH_DUMP"))
		return AL_FLASH_OBJ_ID_CRASH_DUMP;
	else if (!al_strcmp(str, "VPD"))
		return AL_FLASH_OBJ_ID_VPD;
	else if (!al_strcmp(str, "SERDES_25G_FW"))
		return AL_FLASH_OBJ_ID_SERDES_25G_FW;
	else if (!al_strcmp(str, "SERDES_AVG_PCIE_FW"))
		return AL_FLASH_OBJ_ID_SERDES_AVG_PCIE_FW;
	else if (!al_strcmp(str, "SERDES_AVG_NON_PCIE_FW"))
		return AL_FLASH_OBJ_ID_SERDES_AVG_NON_PCIE_FW;
	else if (!al_strcmp(str, "AVG_SBUS_MASTER_FW"))
		return AL_FLASH_OBJ_ID_AVG_SBUS_MASTER_FW;
	else if (!al_strcmp(str, "PUK0"))
		return AL_FLASH_OBJ_ID_PUK0;
	else if (!al_strcmp(str, "PUK1"))
		return AL_FLASH_OBJ_ID_PUK1;
	else if (!al_strcmp(str, "PUK2"))
		return AL_FLASH_OBJ_ID_PUK2;
	else if (!al_strcmp(str, "ATF_BL31"))
		return AL_FLASH_OBJ_ID_ATF_BL31;
	else if (!al_strcmp(str, "ATF_BL32"))
		return AL_FLASH_OBJ_ID_ATF_BL32;
	else if (!al_strcmp(str, "EEPROM"))
		return AL_FLASH_OBJ_ID_EEPROM;
	else if (!al_strcmp(str, "SPIS_PLD"))
		return AL_FLASH_OBJ_ID_SPIS_PLD;

	return -EINVAL;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_id_from_str(
	const char *str)
{
	unsigned int i;
	int obj_id;

	for (i = 0; str[i] && sig_str_hdr[i] && (str[i] == sig_str_hdr[i]); i++)
		;

	if (sig_str_hdr[i])
		return _al_flash_obj_id_from_str(str);

	obj_id = _al_flash_obj_id_from_str(&str[i]);
	if (obj_id < 0)
		return obj_id;

	return AL_FLASH_OBJ_ID_ADD_SIG(obj_id);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_search(
	al_flash_dev_read	dev_read_func,
	unsigned int	first_offset_on_dev,
	unsigned int	skip_size,
	unsigned int	max_num_skips,
	unsigned int	*found_offset_on_dev)
{
	unsigned int offset_on_dev;
	unsigned int i;

	for (offset_on_dev = first_offset_on_dev,
		i = 0; i < max_num_skips;
		offset_on_dev += skip_size, i++) {
		int err;
		unsigned int total_size;
		unsigned int num_entries;

		err = al_flash_toc_validate(dev_read_func, offset_on_dev,
				&total_size, &num_entries);
		if (!err)
			break;
	}

	if (i >= max_num_skips)
		return -EINVAL;

	*found_offset_on_dev = offset_on_dev;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	unsigned int	*total_size,
	unsigned int	*num_entries)
{
	uint32_t csum;
	unsigned int i;
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	uint32_t entries_csum_le;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	if (swap32_from_le(hdr.magic_num) != AL_FLASH_TOC_MAGIC_NUM) {
		al_dbg("%s: flash toc header magic num validation failed!\n", __func__);
		return -EIO;
	} else if (chksum32(&hdr, sizeof(struct al_flash_toc_hdr) -
		sizeof(uint32_t)) != swap32_from_le(hdr.checksum)) {
		al_dbg("%s: flash toc header checksum validation failed!\n",
				__func__);
		return -EIO;
	}

	*num_entries = swap32_from_le(hdr.num_entries);
	*total_size = sizeof(struct al_flash_toc_hdr) + (*num_entries) *
		sizeof(struct al_flash_toc_entry) + sizeof(uint32_t);

	for (i = 0, csum = 0; i < (*num_entries); i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		csum += chksum32(&entry, sizeof(struct al_flash_toc_entry));
	}

	DEV_READ(&entries_csum_le, sizeof(uint32_t));

	if (csum != swap32_from_le(entries_csum_le)) {
		al_err("%s: flash toc entries checksum validation failed!\n", __func__);
		return -EIO;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
static int _al_flash_validate_obj(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		obj_id,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*obj_hdr)
{
	int err;
	switch (AL_FLASH_OBJ_ID_ID(obj_id)) {
	case AL_FLASH_OBJ_ID_STG2:
		err = al_flash_stage2_v2_validate(dev_read_func, offset_on_dev,
			temp_buff, temp_buff_size, obj_hdr);
		if (err)
			/* maybe it's V1? */
			err = al_flash_stage2_validate(dev_read_func, offset_on_dev,
					temp_buff, temp_buff_size, obj_hdr);
		break;
	case AL_FLASH_OBJ_ID_PRE_BOOT:
		err = al_flash_pre_boot_validate(dev_read_func, offset_on_dev,
				temp_buff, temp_buff_size, obj_hdr);
		break;
	case AL_FLASH_OBJ_ID_PRE_BOOT_V2:
		err = al_flash_pre_boot_v2_validate(dev_read_func, offset_on_dev,
			temp_buff, temp_buff_size, obj_hdr);
		break;
	case AL_FLASH_OBJ_ID_HW_PLD:
		err = al_flash_hw_pld_validate(dev_read_func, offset_on_dev,
			temp_buff, temp_buff_size, obj_hdr);
		break;
	default:
		err = al_flash_obj_validate(dev_read_func, offset_on_dev,
			temp_buff, temp_buff_size, obj_hdr);
	}
	return err;
}

/*******************************************************************************
 ******************************************************************************/
static int _al_flash_toc_print(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	void		*temp_buff,
	unsigned int	temp_buff_size,
	al_bool is_extended)
{
	unsigned int i;
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;
	uint8_t	obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN + 1];
	struct al_flash_obj_hdr obj_hdr;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	if (is_extended) {
		al_print(
			"#  | OBJ ID                    | Instance | Name     | Device  | Offset   | Max Size | Valid | Version\n");
		al_print(
			"------------------------------------------------------------------------------------------------------\n");
	} else {
		al_print(
			"#  | OBJ ID                    | Instance | Name     | Device  | Offset   | Max Size\n");
		al_print(
			"------------------------------------------------------------------------------------\n");
	}

	for (i = 0; i < num_entries; i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));

		al_memcpy(obj_id_str, entry.obj_id_str, AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN);
		obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN] = 0;

		al_print("%2d | %06lx (%16s) | %8lu | %8s | %7s | %08x | %08x",
			i,
			AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id))),
			AL_FLASH_OBJ_ID_INSTANCE_NUM(swap32_from_le(entry.obj_id)),
			obj_id_str,
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_CURRENT) ? "Current" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_SPI) ? "SPI" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_NAND) ? "NAND" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_DRAM) ? "DRAM" :
			"N/A",
			swap32_from_le(entry.offset),
			swap32_from_le(entry.max_size));

		if (is_extended) {
			err = _al_flash_validate_obj(
				dev_read_func, entry.offset, entry.obj_id, temp_buff,
				temp_buff_size, &obj_hdr);
			if (err) {
				obj_hdr.major_ver = 0;
				obj_hdr.minor_ver = 0;
				obj_hdr.fix_ver = 0;
			}
			al_print(" | %5c | %u.%u.%u",
				(err) ? 'X' : 'V', obj_hdr.major_ver,
				obj_hdr.minor_ver, obj_hdr.fix_ver);
		}
		al_print("\n");
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_print(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev)
{
	return _al_flash_toc_print(dev_read_func, offset_on_dev, NULL, 0, AL_FALSE);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_print_ex(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	void		*temp_buff,
	unsigned int	temp_buff_size)
{
	return _al_flash_toc_print(
		dev_read_func, offset_on_dev, temp_buff, temp_buff_size, AL_TRUE);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_info_print(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	unsigned int	obj_id,
	void		*temp_buff,
	unsigned int	temp_buff_size)
{
	int err;
	struct al_flash_obj_hdr hdr;
	uint8_t	desc[AL_FLASH_OBJ_DESC_LEN + 1];

	err = _al_flash_validate_obj(dev_read_func, offset_on_dev, obj_id, temp_buff,
								temp_buff_size, &hdr);
	if (err) {
		al_print("ID = %06lx (%s) - Object failed validation!\n",
			AL_FLASH_OBJ_ID_ID(obj_id),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID((obj_id))));
		return -EINVAL;
	}

	al_print("ID = %06lx (%s)\n",
		AL_FLASH_OBJ_ID_ID(hdr.id),
		al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID((hdr.id))));

	al_memcpy(desc, hdr.desc, AL_FLASH_OBJ_DESC_LEN);
	desc[AL_FLASH_OBJ_DESC_LEN] = 0;
	al_print("Description = %s\n", desc);

	al_print("Version = %u.%u.%u\n", hdr.major_ver, hdr.minor_ver, hdr.fix_ver);

	al_print("Size = %08x\n", hdr.size);

	al_print("Load address = %08x%08x\n", hdr.load_addr_hi, hdr.load_addr_lo);

	al_print("Exec address = %08x%08x\n", hdr.exec_addr_hi, hdr.exec_addr_lo);

	al_print("Flags = %08x\n", hdr.flags);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_entry_get(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		index,
	struct al_flash_toc_entry	*found_entry)
{
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;

	al_dbg("%s(%X, %u)\n", __func__, offset_on_dev, index);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	if (index >= num_entries) {
		al_err("%s: index exceeds the number of entries!\n", __func__);
		return -EINVAL;
	}

	offset_on_dev += index * sizeof(struct al_flash_toc_entry);

	DEV_READ(&entry, sizeof(struct al_flash_toc_entry));

	al_memcpy(found_entry, &entry, sizeof(struct al_flash_toc_entry));
	found_entry->obj_id = swap32_from_le(entry.obj_id);
	found_entry->dev_id = swap32_from_le(entry.dev_id);
	found_entry->offset = swap32_from_le(entry.offset);
	found_entry->max_size = swap32_from_le(entry.max_size);
	found_entry->flags = swap32_from_le(entry.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_find_id(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	uint32_t		obj_id,
	unsigned int		first_index,
	int			*found_index,
	struct al_flash_toc_entry	*found_entry)
{
	return al_flash_toc_find_id_with_fallback(dev_read_func, offset_on_dev, obj_id, obj_id,
		first_index, found_index, found_entry);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_find_id_with_fallback(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	uint32_t		obj_id,
	uint32_t		obj_fallback_id,
	unsigned int		first_index,
	int			*found_index,
	struct al_flash_toc_entry	*found_entry)
{
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	struct al_flash_toc_entry fallback_entry;
	unsigned int num_entries;
	int fallback_index = -1;

	al_dbg("%s(%X, %u, %u, %u)\n",
		__func__, offset_on_dev, obj_id, obj_fallback_id, first_index);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	offset_on_dev += first_index * sizeof(struct al_flash_toc_entry);

	for (; first_index < num_entries; first_index++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		al_dbg("%s: obj_id = %d\n", __func__, swap32_from_le(entry.obj_id));
		if (obj_id == swap32_from_le(entry.obj_id))
			break;
		if (obj_fallback_id == swap32_from_le(entry.obj_id)) {
			fallback_index = first_index;
			fallback_entry = entry;
		}
	}

	if ((first_index >= num_entries) && (fallback_index >= 0)) {
		al_dbg("%s: Falling back to fallback object (%s #%u)...\n", __func__,
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_fallback_id)),
			(unsigned int)AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_fallback_id));
		first_index = fallback_index;
		entry = fallback_entry;
	}

	if (first_index < num_entries) {
		*found_index = first_index;
		al_memcpy(found_entry, &entry, sizeof(struct al_flash_toc_entry));
		found_entry->obj_id = swap32_from_le(entry.obj_id);
		found_entry->dev_id = swap32_from_le(entry.dev_id);
		found_entry->offset = swap32_from_le(entry.offset);
		found_entry->max_size = swap32_from_le(entry.max_size);
		found_entry->flags = swap32_from_le(entry.flags);
	} else {
		*found_index = -1;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_stage2_active_instance_get(
	void			*pbs_sram_base,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num)
{
	return al_flash_toc_stage2_active_instance_get_by_off(
			pbs_sram_base,
			STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM,
			dev_read_func,
			offset_on_dev,
			instance_num);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_stage2_active_instance_get_by_off(
	void			*pbs_sram_base,
	unsigned int		stage2_actual_offset_addr_in_pbs_sram,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num)
{
	unsigned int instance_fallback_num;

	return al_flash_toc_stage2_active_instance_get_with_fallback(
			pbs_sram_base,
			stage2_actual_offset_addr_in_pbs_sram,
			dev_read_func,
			offset_on_dev,
			instance_num,
			&instance_fallback_num);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_stage2_active_instance_get_with_fallback(
	void			*pbs_sram_base,
	unsigned int		stage2_actual_offset_addr_in_pbs_sram,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num,
	unsigned int		*instance_fallback_num)
{
	int err;
	uint8_t *pbs_sram_base_ptr = (uint8_t *)pbs_sram_base;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;
	uint32_t stage2_actual_offset;
	unsigned int i;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	stage2_actual_offset = al_reg_read32(
		(uint32_t *)(pbs_sram_base_ptr + stage2_actual_offset_addr_in_pbs_sram));

	if (stage2_actual_offset & FORCED_INSTANCE_NUM_MARK) {
		*instance_num =
			(stage2_actual_offset & FORCED_INSTANCE_NUM_NUM_MASK) >>
			FORCED_INSTANCE_NUM_NUM_SHIFT;
		*instance_fallback_num =
			(stage2_actual_offset & FORCED_INSTANCE_NUM_FALLBACK_NUM_MASK) >>
			FORCED_INSTANCE_NUM_FALLBACK_NUM_SHIFT;
		return 0;
	}

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	for (i = 0; i < num_entries; i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		if (((AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)) == AL_FLASH_OBJ_ID_STG2) ||
			(AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)) ==
			AL_FLASH_OBJ_ID_PRE_BOOT) ||
			(AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)) ==
			AL_FLASH_OBJ_ID_PRE_BOOT_V2)) &&
			(swap32_from_le(entry.offset) == stage2_actual_offset)) {
			*instance_num = AL_FLASH_OBJ_ID_INSTANCE_NUM(swap32_from_le(entry.obj_id));
			*instance_fallback_num = *instance_num;
			break;
		}
	}

	if (i >= num_entries) {
		al_err("%s: unable to find stage 2 at offset %08x\n", __func__, stage2_actual_offset);
		return -EINVAL;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
void al_flash_toc_stage2_active_instance_force_temporary(
	void			*pbs_sram_base,
	unsigned int		stage2_actual_offset_addr_in_pbs_sram,
	unsigned int		instance_num,
	unsigned int		instance_fallback_num)
{
	uint8_t *pbs_sram_base_ptr = (uint8_t *)pbs_sram_base;

	al_dbg("%s(%u, %u)\n", __func__, instance_num, instance_fallback_num);

	al_reg_write32(
		(uint32_t *)(pbs_sram_base_ptr + stage2_actual_offset_addr_in_pbs_sram),
		FORCED_INSTANCE_NUM_MARK |
		(instance_num << FORCED_INSTANCE_NUM_NUM_SHIFT) |
		(instance_fallback_num << FORCED_INSTANCE_NUM_FALLBACK_NUM_SHIFT));
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_header_read_and_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_dbg("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	} else if (swap32_from_le(hdr.magic_num) != AL_FLASH_OBJ_MAGIC_NUM) {
		al_err("%s: flash obj header contains invalid magic num\n", __func__);
		al_err("\texpected 0x%X, read 0x%X\n",
			AL_FLASH_OBJ_MAGIC_NUM,
			swap32_from_le(hdr.magic_num));
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->fix_ver = swap32_from_le(hdr.fix_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_data_write(
	void			*entry,
	void			*data,
	size_t			size)
{
	struct al_flash_obj_hdr	*hdr = (struct al_flash_obj_hdr	*)entry;
	uint8_t *entry_data;
	uint32_t *entry_checksum;

	if (chksum32(hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr->checksum)) {
		al_dbg("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	} else if (swap32_from_le(hdr->magic_num) != AL_FLASH_OBJ_MAGIC_NUM) {
		al_err("%s: flash obj header contains invalid magic num\n", __func__);
		al_err("\texpected 0x%X, read 0x%X\n",
			AL_FLASH_OBJ_MAGIC_NUM,
			swap32_from_le(hdr->magic_num));
		return -EIO;
	}

	hdr->size = swap32_to_le(size);
	hdr->checksum = swap32_to_le(chksum32(hdr,
					      sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)));

	entry_data = (uint8_t *)entry + sizeof(struct al_flash_obj_hdr);
	al_memcpy(entry_data, data, size);

	entry_checksum = (uint32_t *)(entry_data + size);
	*entry_checksum = swap32_to_le(chksum32(entry_data, size));

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_flash_obj_data_offset_get(
	unsigned int		obj_offset_on_dev)
{
	return obj_offset_on_dev + sizeof(struct al_flash_obj_hdr);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_data_load(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*load_addr)
{
	uint32_t csum;
	int err;
	struct al_flash_obj_hdr hdr;
	unsigned int size;
	uint32_t data_csum_le;

	al_dbg("%s(%X, %p)\n", __func__, offset_on_dev, load_addr);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	size = swap32_from_le(hdr.size);

	DEV_READ(load_addr, size);

	DEV_READ(&data_csum_le, sizeof(uint32_t));

	csum = chksum32(load_addr, size);
	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_validate(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;
	uint32_t csum = 0;
	unsigned int size;
	uint32_t data_csum_le;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (swap32_from_le(hdr.magic_num) != AL_FLASH_OBJ_MAGIC_NUM) {
		al_dbg("%s: flash obj header contains invalid magic num\n", __func__);
		al_dbg("\texpected 0x%X, read 0x%X\n",
			AL_FLASH_OBJ_MAGIC_NUM,
			swap32_from_le(hdr.magic_num));
		return -EIO;
	}

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_dbg("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	size = swap32_from_le(hdr.size);

	while (size) {
		unsigned int size_current =
			(size < temp_buff_size) ? size : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum32(temp_buff, size_current);
		size -= size_current;
	}

	DEV_READ(&data_csum_le, sizeof(uint32_t));

	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
static int al_flash_obj_hdr_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	al_bool			*is_valid,
	struct al_flash_obj_hdr	*hdr)
{
	int err;

	*is_valid = AL_FALSE;

	DEV_READ(hdr, sizeof(struct al_flash_obj_hdr));

	if (swap32_from_le(hdr->magic_num) != AL_FLASH_OBJ_MAGIC_NUM)
		return 0;

	if (chksum32(hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr->checksum)) {
		return 0;
	}

	*is_valid = AL_TRUE;

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
static int _al_flash_stage2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr,
	al_bool			v1)
{
	int err;
	struct al_flash_obj_hdr new_hdr;
	unsigned int size;
	uint16_t csum = 0;
	uint16_t data_csum_le;
	al_bool hdr_valid;

	al_dbg("%s(%X, %p, %u)\n", __func__, offset_on_dev, temp_buff, temp_buff_size);

	if (v1) {
		struct al_flash_stage2_hdr hdr;

		DEV_READ(&hdr, sizeof(struct al_flash_stage2_hdr));

		if (chksum16(&hdr, sizeof(struct al_flash_stage2_hdr) - sizeof(uint16_t)) !=
			swap16_from_le(hdr.checksum)) {
			al_dbg("%s: flash stage2 header checksum validation failed!\n", __func__);
			return -EIO;
		}

		size = swap16_from_le(hdr.size);
	} else {
		struct al_flash_stage2_v2_hdr hdr;

		DEV_READ(&hdr, sizeof(struct al_flash_stage2_v2_hdr));

		if (chksum16(&hdr, sizeof(struct al_flash_stage2_v2_hdr) - sizeof(uint16_t))
				!= swap16_from_le(hdr.checksum)) {
			al_dbg("%s: flash stage2 header checksum validation failed!\n", __func__);
			return -EIO;
		}

		size = swap32_from_le(hdr.size);
	}

	while (size) {
		unsigned int size_current =
			(size < temp_buff_size) ? size : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum16(temp_buff, size_current);
		size -= size_current;
	}

	DEV_READ(&data_csum_le, sizeof(uint16_t));

	if (csum != swap16_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	err = al_flash_obj_hdr_validate(dev_read_func, offset_on_dev, &hdr_valid, &new_hdr);
	if (err)
		return err;

	if (!hdr_valid) {
		err = al_flash_obj_hdr_validate(dev_read_func,
			offset_on_dev + STAGE2_SIGNATURE_AND_MODULUS_SIZE, &hdr_valid, &new_hdr);
		if (err)
			return err;
		if (!hdr_valid) {
			al_err("%s: flash header validation failed!", __func__);
			return -EIO;
		}
	}

	al_memcpy(read_hdr, &new_hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(new_hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(new_hdr.format_rev_id);
	read_hdr->id = swap32_from_le(new_hdr.id);
	read_hdr->major_ver = swap32_from_le(new_hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(new_hdr.minor_ver);
	read_hdr->size = swap32_from_le(new_hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(new_hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(new_hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(new_hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(new_hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(new_hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_stage2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	return _al_flash_stage2_validate(dev_read_func, offset_on_dev, temp_buff, temp_buff_size,
		read_hdr, AL_TRUE);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_stage2_v2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	return _al_flash_stage2_validate(dev_read_func, offset_on_dev, temp_buff, temp_buff_size,
		read_hdr, AL_FALSE);
}

/*******************************************************************************
 ******************************************************************************/
static int _pre_boot_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		header_offset,
	unsigned int		post_header_offset,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;
	uint32_t csum = 0;
	unsigned int size;
	unsigned int size_before_header;
	uint32_t data_csum_le;
	unsigned int offset_on_dev_orig = offset_on_dev;

	al_dbg("%s(%08x, %08x, %08x)\n",
		__func__, offset_on_dev, header_offset, post_header_offset);

	offset_on_dev += header_offset;

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_dbg("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	offset_on_dev = offset_on_dev_orig;

	size = swap32_from_le(hdr.size);

	size_before_header = header_offset;
	size -= size_before_header;

	while (size_before_header) {
		unsigned int size_current =
			(size_before_header < temp_buff_size) ? size_before_header : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum32(temp_buff, size_current);
		size_before_header -= size_current;
	}

	offset_on_dev = offset_on_dev_orig + post_header_offset;

	while (size) {
		unsigned int size_current =
			(size < temp_buff_size) ? size : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum32(temp_buff, size_current);
		size -= size_current;
	}

	DEV_READ(&data_csum_le, sizeof(uint32_t));

	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_pre_boot_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	return _pre_boot_validate(
		dev_read_func, offset_on_dev, AL_FLASH_PRE_BOOT_HEADER_OFFSET,
		AL_FLASH_PRE_BOOT_STG2_5_OFFSET, temp_buff, temp_buff_size, read_hdr);
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_pre_boot_v2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	return _pre_boot_validate(
		dev_read_func, offset_on_dev, AL_FLASH_PRE_BOOT_V2_HEADER_OFFSET,
		AL_FLASH_PRE_BOOT_V2_STG3_OFFSET, temp_buff, temp_buff_size, read_hdr);
}

/*******************************************************************************
 ******************************************************************************/
#define HW_PLD_HDR_FTR_ENT_ADDR	0x1c361c361c361c36ULL
#define HW_PLD_HDR_FTR_ENT_DATA	0x1c361c36
#define HW_PLD_NUM_ENTRIES_MAX	1000

int al_flash_hw_pld_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;
	unsigned int size;
	uint32_t csum = 0;
	uint32_t data_csum_le;
	uint32_t entry_data;
	uint64_t entry_addr;
	al_bool got_hdr = AL_FALSE;
	al_bool got_footer = AL_FALSE;
	unsigned int num_entries;

	al_dbg("%s(%X, %p, %u)\n", __func__, offset_on_dev, temp_buff, temp_buff_size);

	for (num_entries = 0, got_hdr = AL_FALSE, got_footer = AL_FALSE, size = 0;
		(num_entries <= HW_PLD_NUM_ENTRIES_MAX) && (!got_footer);
		num_entries++, size += sizeof(entry_data) + sizeof(entry_addr)) {
		DEV_READ(&entry_data, sizeof(entry_data));
		csum += chksum32(&entry_data, sizeof(entry_data));
		DEV_READ(&entry_addr, sizeof(entry_addr));
		csum += chksum32(&entry_addr, sizeof(entry_addr));

		if (!got_hdr) {
			if (swap64_from_le(entry_addr) != HW_PLD_HDR_FTR_ENT_ADDR) {
				al_err("%s: unexpected header entry address (%08x%08x)!\n",
					__func__, AL_UINT64_HIGH(swap64_from_le(entry_addr)),
					AL_UINT64_LOW(swap64_from_le(entry_addr)));
				return -EINVAL;
			}
			if (swap32_from_le(entry_data) != HW_PLD_HDR_FTR_ENT_DATA) {
				al_err("%s: unexpected header entry data (%08x)!\n", __func__,
					swap32_from_le(entry_data));
				return -EINVAL;
			}
			got_hdr = AL_TRUE;
		} else {
			if (swap64_from_le(entry_addr) != HW_PLD_HDR_FTR_ENT_ADDR)
				continue;
			if (swap32_from_le(entry_data) != HW_PLD_HDR_FTR_ENT_DATA)
				continue;
			got_footer = AL_TRUE;
		}
	}

	if (!got_footer) {
		al_err("%s: didn't encounter footer!\n", __func__);
		return -EINVAL;
	}

	DEV_READ(&data_csum_le, sizeof(uint32_t));
	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));
	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_err("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}
	if (size != swap32_from_le(hdr.size)) {
		al_err("%s: invalid size!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
static int al_flash_mem_read(unsigned int offset, void *address, unsigned int size)
{
	al_memcpy(address, (void *)(uintptr_t)offset, size);
	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static int al_flash_obj_temp_read(
	al_flash_dev_read	dev_read_func,
	unsigned int		toc_offset_on_dev,
	uint32_t		obj_id,
	uint8_t			*fimg_buff,
	unsigned int		fimg_buff_size_max,
	uint8_t			*temp_buff,
	unsigned int		temp_buff_size,
	unsigned int		*fimg_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	int index;
	struct al_flash_toc_entry toc_entry;
	struct al_flash_obj_hdr hdr;
	unsigned int size;
	unsigned int offset_on_dev;

	err = al_flash_toc_find_id(dev_read_func, toc_offset_on_dev, obj_id, 0, &index, &toc_entry);
	if (err) {
		al_err("%s: Cannot find object (%s #%u)...\n", __func__,
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_id)),
			(unsigned int)AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_id));
		return err;
	}

	offset_on_dev = toc_entry.offset;

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (swap32_from_le(hdr.magic_num) != AL_FLASH_OBJ_MAGIC_NUM) {
		al_err("%s: flash obj header contains invalid magic num\n", __func__);
		al_err("\texpected 0x%X, read 0x%X\n",
			AL_FLASH_OBJ_MAGIC_NUM,
			swap32_from_le(hdr.magic_num));
		return -EIO;
	}

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_err("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	if (swap32_from_le(hdr.id) != AL_FLASH_OBJ_ID_ID(obj_id)) {
		al_err("%s: flash obj header contains invalid object ID (%08x != %08x)!\n",
			__func__, (unsigned int)AL_FLASH_OBJ_ID_ID(obj_id),
			(unsigned int)swap32_from_le(hdr.id));
		return -EIO;
	}

	size = swap32_from_le(hdr.size);

	if (fimg_buff_size_max < (sizeof(hdr) + size + sizeof(uint32_t))) {
		al_err("%s: fimg buff size not sufficient!\n", __func__);
		return -EINVAL;
	}

	al_memcpy(fimg_buff, &hdr, sizeof(hdr));
	DEV_READ(fimg_buff + sizeof(hdr), size + sizeof(uint32_t));

	err = al_flash_obj_validate(al_flash_mem_read, (uintptr_t)fimg_buff, temp_buff,
		temp_buff_size, read_hdr);
	if (err) {
		al_err("%s: failed validating object (%08x %s)!\n", __func__,
			(unsigned int)AL_FLASH_OBJ_ID_ID(obj_id),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_id)));
		return err;
	}

	*fimg_buff_size = sizeof(struct al_flash_obj_hdr) + size + sizeof(uint32_t);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
#define AUTH_TEMP_BUFF_OFFSET_TEMP	0
#define AUTH_TEMP_BUFF_SIZE_TEMP	0x1000
#define AUTH_TEMP_BUFF_OFFSET_SIG_FIMG	0x1000
#define AUTH_TEMP_BUFF_SIZE_SIG_FIMG	0x1000
#define AUTH_TEMP_BUFF_OFFSET_PUK_FIMG	0x2000
#define AUTH_TEMP_BUFF_SIZE_PUK_FIMG	0x1000
#define AUTH_TEMP_BUFF_OFFSET_FIMG	0x3000

#define PUK_IDX_INVALID			0xffffffff

static unsigned int _al_flash_obj_puk_idx_get(
	uint32_t				obj_id)
{
	switch (AL_FLASH_OBJ_ID_ID(obj_id)) {
	case AL_FLASH_OBJ_ID_PUK0:
		return 0;
	case AL_FLASH_OBJ_ID_PUK1:
		return 1;
	case AL_FLASH_OBJ_ID_PUK2:
		return 2;
	}

	return PUK_IDX_INVALID;
}

static int _al_flash_obj_auth(
	al_flash_dev_read			dev_read_func,
	unsigned int				toc_offset_on_dev,
	uint32_t				obj_id,
	void					*temp_buff,
	unsigned int				temp_buff_size,
	al_flash_buff_auth_hash_in_efuse	auth_hash_in_efuse,
	al_flash_buff_auth_pkcs_1_v15		auth_pkcs_1_v15,
	al_flash_obj_ver_auth			auth_ver,
	uint8_t					**authenticated_fimg_buff,
	unsigned int				*authenticated_fimg_buff_size,
	struct al_flash_obj_hdr			*authenticated_fimg_hdr)
{
	int err;
	unsigned int sig_obj_id;
	unsigned int puk_obj_id;
	uint8_t *tmp_buff = ((uint8_t *)temp_buff) + AUTH_TEMP_BUFF_OFFSET_TEMP;
	uint8_t *fimg_buff = ((uint8_t *)temp_buff) + AUTH_TEMP_BUFF_OFFSET_FIMG;
	unsigned int fimg_buff_size_max = temp_buff_size - AUTH_TEMP_BUFF_OFFSET_FIMG;
	unsigned int fimg_buff_size;
	uint8_t *sig_fimg_buff = ((uint8_t *)temp_buff) + AUTH_TEMP_BUFF_OFFSET_SIG_FIMG;
	unsigned int sig_fimg_buff_size;
	uint8_t *puk_fimg_buff = ((uint8_t *)temp_buff) + AUTH_TEMP_BUFF_OFFSET_PUK_FIMG;
	unsigned int puk_fimg_buff_size;
	struct al_flash_obj_hdr	obj_hdr;
	struct al_flash_obj_hdr	sig_obj_hdr;
	struct al_flash_obj_hdr	puk_obj_hdr;
	struct al_flash_sig_hdr *sig_hdr;
	void *sig_data;
	unsigned int sig_size;
	struct al_flash_puk_hdr *puk_hdr;
	void *puk_data;
	unsigned int puk_size;
	static const uint8_t public_exponent[] = {0x01, 0x00, 0x01};
	unsigned int instance_num = AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_id);
	al_bool puk_hash_in_efuse;
	al_bool obj_is_puk;
	unsigned int puk_idx;
	unsigned int puk_idx_expected = 0;

	al_assert(temp_buff_size > AUTH_TEMP_BUFF_OFFSET_FIMG);

	/* Read object */

	al_info("%s: Reading object (%s(%08x) #%u)\n", __func__,
		al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_id)),
		(unsigned int)AL_FLASH_OBJ_ID_ID(obj_id),
		instance_num);

	err = al_flash_obj_temp_read(dev_read_func, toc_offset_on_dev, obj_id, fimg_buff,
		fimg_buff_size_max, tmp_buff, AUTH_TEMP_BUFF_SIZE_TEMP, &fimg_buff_size, &obj_hdr);
	if (err) {
		al_err("%s: failed reading object ID (%u %s)!\n", __func__,
			(unsigned int)AL_FLASH_OBJ_ID_ID(obj_id),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_id)));
		return err;
	}

	puk_idx = _al_flash_obj_puk_idx_get(obj_id);
	if (!puk_idx) {
		al_err("%s: Non supported object ID (%s(%08x) #%u)!\n", __func__,
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(obj_id)),
			(unsigned int)AL_FLASH_OBJ_ID_ID(obj_id),
			(unsigned int)AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_id));
		return -EINVAL;
	} else if (puk_idx != PUK_IDX_INVALID) {
		obj_is_puk = AL_TRUE;
		puk_idx_expected = puk_idx - 1;
	} else {
		obj_is_puk = AL_FALSE;
	}

	sig_obj_id = AL_FLASH_OBJ_ID_ADD_SIG(obj_id);

	/* Read signature */

	al_info("%s: Reading object (%s(%08x) #%u)\n", __func__,
		al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(sig_obj_id)),
		(unsigned int)AL_FLASH_OBJ_ID_ID(sig_obj_id),
		instance_num);

	err = al_flash_obj_temp_read(dev_read_func, toc_offset_on_dev, sig_obj_id,
		sig_fimg_buff, AUTH_TEMP_BUFF_SIZE_SIG_FIMG, tmp_buff,
		AUTH_TEMP_BUFF_SIZE_TEMP, &sig_fimg_buff_size, &sig_obj_hdr);
	if (err) {
		al_err("%s: failed reading object ID (%u %s)!\n", __func__,
			(unsigned int)AL_FLASH_OBJ_ID_ID(sig_obj_id),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(sig_obj_id)));
		return err;
	}

	if (swap32_from_le(sig_obj_hdr.size) < sizeof(struct al_flash_sig_hdr)) {
		al_err("%s: Too small signature object!\n", __func__);
		return -EINVAL;
	}

	sig_hdr = (struct al_flash_sig_hdr *)(sig_fimg_buff + sizeof(struct al_flash_obj_hdr));
	sig_data = (void *)(sig_fimg_buff + sizeof(struct al_flash_obj_hdr) +
		sizeof(struct al_flash_sig_hdr));

	switch (swap32_from_le(sig_hdr->type)) {
	case AL_FLASH_SIG_TYPE_PKCS_1_V15_RSA_2K_SHA_256:
		sig_size = 256;
		if (swap32_from_le(sig_obj_hdr.size) !=
				(sizeof(struct al_flash_sig_hdr) + sig_size)) {
			al_err("%s: Invalid signature object size!\n", __func__);
			return -EINVAL;
		}
		break;
	default:
		al_err("%s: Non supported signature type (%u)!\n", __func__,
			swap32_from_le(sig_hdr->type));
		return -EINVAL;
	}

	if (obj_is_puk && (puk_idx_expected != swap32_from_le(sig_hdr->puk_idx))) {
		al_err("%s: Non expected PUK index (%u != %u)!\n", __func__,
			puk_idx_expected, swap32_from_le(sig_hdr->puk_idx));
		return -EINVAL;
	}

	puk_hash_in_efuse = AL_FALSE;
	switch (swap32_from_le(sig_hdr->puk_idx)) {
	case 0:
		puk_obj_id = AL_FLASH_OBJ_ID(AL_FLASH_OBJ_ID_PUK0, instance_num);
		puk_hash_in_efuse = AL_TRUE;
		break;
	case 1:
		puk_obj_id = AL_FLASH_OBJ_ID(AL_FLASH_OBJ_ID_PUK1, instance_num);
		break;
	case 2:
		puk_obj_id = AL_FLASH_OBJ_ID(AL_FLASH_OBJ_ID_PUK2, instance_num);
		break;
	default:
		al_err("%s: Non supported PUK index (%u)!\n", __func__,
			swap32_from_le(sig_hdr->puk_idx));
		return -EINVAL;
	}

	/* Read/Authenticate PUK */

	if (puk_hash_in_efuse) {
		al_info("%s: Reading object (%s(%08x) #%u)\n", __func__,
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(puk_obj_id)),
			(unsigned int)AL_FLASH_OBJ_ID_ID(puk_obj_id),
			instance_num);

		err = al_flash_obj_temp_read(dev_read_func, toc_offset_on_dev, puk_obj_id,
			puk_fimg_buff, AUTH_TEMP_BUFF_SIZE_PUK_FIMG, tmp_buff,
			AUTH_TEMP_BUFF_SIZE_TEMP, &puk_fimg_buff_size, &puk_obj_hdr);
		if (err) {
			al_err("%s: failed reading object ID (%u %s)!\n", __func__,
				(unsigned int)AL_FLASH_OBJ_ID_ID(puk_obj_id),
				al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(puk_obj_id)));
			return err;
		}
	} else {
		al_info("%s: Authenticating PUK (%s(%08x) #%u)\n", __func__,
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(puk_obj_id)),
			(unsigned int)AL_FLASH_OBJ_ID_ID(puk_obj_id), instance_num);

		err = _al_flash_obj_auth(dev_read_func, toc_offset_on_dev, puk_obj_id,
			fimg_buff + fimg_buff_size, temp_buff_size - (fimg_buff_size +
			AUTH_TEMP_BUFF_OFFSET_FIMG), auth_hash_in_efuse, auth_pkcs_1_v15, auth_ver,
			&puk_fimg_buff, &puk_fimg_buff_size, &puk_obj_hdr);
		if (err) {
			al_err("%s: Authentication failed!\n", __func__);
			return err;
		}
	}

	if (swap32_from_le(puk_obj_hdr.size) < sizeof(struct al_flash_puk_hdr)) {
		al_err("%s: Too small public key object!\n", __func__);
		return -EINVAL;
	}

	puk_hdr = (struct al_flash_puk_hdr *)(puk_fimg_buff + sizeof(struct al_flash_obj_hdr));
	puk_data = (void *)(puk_fimg_buff + sizeof(struct al_flash_obj_hdr) +
		sizeof(struct al_flash_puk_hdr));

	if (swap32_from_le(puk_hdr->type) == AL_FLASH_PUK_TYPE_RSA_2K) {
		puk_size = 256;
		if (swap32_from_le(puk_obj_hdr.size) !=
				(sizeof(struct al_flash_puk_hdr) + puk_size)) {
			al_err("%s: Invalid public key object size!\n", __func__);
			return -EINVAL;
		}

		/* Authenticate the PUK using hash in efuse */
		if (puk_hash_in_efuse) {
			al_info("%s: Comparing PUK hash to efuse (%s(%08x) #%u)\n", __func__,
				al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(puk_obj_id)),
				(unsigned int)AL_FLASH_OBJ_ID_ID(puk_obj_id), instance_num);
			err = auth_hash_in_efuse(puk_data, puk_size);
			if (err) {
				al_err("%s: EFuse Authentication failed!\n", __func__);
				return err;
			}
		}

		err = auth_pkcs_1_v15(fimg_buff, fimg_buff_size, sig_data, sig_size,
			public_exponent, sizeof(public_exponent), puk_data, puk_size);
		if (err) {
			al_err("%s: Authentication failed!\n", __func__);
			return err;
		}

		err = auth_ver(&obj_hdr);
		if (err) {
			al_err("%s: Version authentication failed!\n", __func__);
			return err;
		}
	} else {
		al_err("%s: Non supported PUK type (%u)!\n", __func__,
			swap32_from_le(puk_hdr->type));
		return -EINVAL;
	}

	*authenticated_fimg_buff = fimg_buff;
	*authenticated_fimg_buff_size = fimg_buff_size;
	*authenticated_fimg_hdr = obj_hdr;

	return 0;
}

int al_flash_obj_auth(
	al_flash_dev_read			dev_read_func,
	unsigned int				toc_offset_on_dev,
	uint32_t				obj_id,
	void					*temp_buff,
	unsigned int				temp_buff_size,
	al_flash_buff_auth_hash_in_efuse	auth_hash_in_efuse,
	al_flash_buff_auth_pkcs_1_v15		auth_pkcs_1_v15,
	al_flash_obj_ver_auth			auth_ver,
	al_bool					load,
	void					*load_addr,
	struct al_flash_obj_hdr			*authenticated_fimg_hdr)
{
	uint8_t *fimg_buff;
	unsigned int fimg_buff_size;
	struct al_flash_obj_hdr obj_hdr;
	int err;

	err = _al_flash_obj_auth(dev_read_func, toc_offset_on_dev, obj_id, temp_buff,
		temp_buff_size, auth_hash_in_efuse, auth_pkcs_1_v15, auth_ver, &fimg_buff,
		&fimg_buff_size, &obj_hdr);
	if (err)
		return err;

	*authenticated_fimg_hdr = obj_hdr;

	if (load) {
		if (!load_addr)
			load_addr = (void *)(uintptr_t)obj_hdr.load_addr_lo;

		err = al_flash_obj_data_load(al_flash_mem_read, (uintptr_t)fimg_buff, load_addr);
		if (err) {
			al_err("%s: Loading failed!\n", __func__);
			return err;
		}
	}

	return 0;
}
