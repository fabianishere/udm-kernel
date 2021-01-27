#ifndef AL_HAL_ETH_FWD_TABLES_H_
#define AL_HAL_ETH_FWD_TABLES_H_

/* Field access macros */
#define FWD_TABLE_FIELD_DIDX_HELPER(tname, fname)      tname##_FIELD_##fname##_DIDX
#define FWD_TABLE_FIELD_DIDX(tname, fname)             FWD_TABLE_FIELD_DIDX_HELPER(tname, fname)

#define FWD_TABLE_FIELD_FTYPE_HELPER(tname, fname)     tname##_FIELD_##fname##_FTYPE
#define FWD_TABLE_FIELD_FTYPE(tname, fname)            FWD_TABLE_FIELD_FTYPE_HELPER(tname, fname)

#define FWD_TABLE_FIELD_SHIFT_HELPER1(tregname, fname, didx)  \
	tregname##didx##_##fname##_SHIFT
#define FWD_TABLE_FIELD_SHIFT_HELPER2(tregname, fname, didx)  \
	FWD_TABLE_FIELD_SHIFT_HELPER1(tregname, fname, didx)
#define FWD_TABLE_FIELD_SHIFT(tname, tregname, fname)         \
	FWD_TABLE_FIELD_SHIFT_HELPER2(tregname, fname, FWD_TABLE_FIELD_DIDX(tname, fname))

#define FWD_TABLE_FIELD_MASK_HELPER1(tregname, fname, didx)   \
	tregname##didx##_##fname##_MASK
#define FWD_TABLE_FIELD_MASK_HELPER2(tregname, fname, didx)   \
	FWD_TABLE_FIELD_MASK_HELPER1(tregname, fname, didx)
#define FWD_TABLE_FIELD_MASK(tname, tregname, fname)          \
	FWD_TABLE_FIELD_MASK_HELPER2(tregname, fname, FWD_TABLE_FIELD_DIDX(tname, fname))

#define FWD_TABLE_FIELD_BIT_HELPER1(tregname, fname, didx)    \
	tregname##didx##_##fname
#define FWD_TABLE_FIELD_BIT_HELPER2(tregname, fname, didx)    \
	FWD_TABLE_FIELD_BIT_HELPER1(tregname, fname, didx)
#define FWD_TABLE_FIELD_BIT(tname, tregname, fname)           \
	FWD_TABLE_FIELD_BIT_HELPER2(tregname, fname, FWD_TABLE_FIELD_DIDX(tname, fname))

/** Setters */
#define FWD_TABLE_DWORD_SET(entry, didx, value)     \
	((entry)[(didx)] = (value))

#define FWD_TABLE_FIELD_SET_DWORD(tname, tregname, entry, fname, value) \
	((entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] = (value))

#define FWD_TABLE_FIELD_SET_MULTI(tname, tregname, entry, fname, value) do {\
	(entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] &= \
		~(FWD_TABLE_FIELD_MASK(tname, tregname, fname));\
	(entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] |= \
		(((value) << FWD_TABLE_FIELD_SHIFT(tname, tregname, fname)) & \
			FWD_TABLE_FIELD_MASK(tname, tregname, fname));\
} while (0)

#define FWD_TABLE_FIELD_SET_BIT(tname, tregname, entry, fname, value) do {\
	(entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] &= \
		~(FWD_TABLE_FIELD_BIT(tname, tregname, fname));\
	if ((value)) {\
		(entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] |= \
			FWD_TABLE_FIELD_BIT(tname, tregname, fname);\
	} \
} while (0)

#define FWD_TABLE_FIELD_SET_HELPER1(tname, tregname, entry, fname, value, ftype) \
	FWD_TABLE_FIELD_SET_##ftype(tname, tregname, entry, fname, value)
#define FWD_TABLE_FIELD_SET_HELPER2(tname, tregname, entry, fname, value, ftype) \
	FWD_TABLE_FIELD_SET_HELPER1(tname, tregname, entry, fname, value, ftype)
#define FWD_TABLE_FIELD_SET(tname, tregname, entry, fname, value)             \
	FWD_TABLE_FIELD_SET_HELPER2(tname, tregname, entry, fname, value, \
			FWD_TABLE_FIELD_FTYPE(tname, fname))

/** Getters */
#define FWD_TABLE_DWORD_GET(entry, didx)     \
	((entry)[(didx)])

#define FWD_TABLE_FIELD_GET_DWORD(tname, tregname, entry, fname) \
	((entry)[FWD_TABLE_FIELD_DIDX(tname, fname)])

#define FWD_TABLE_FIELD_GET_MULTI(tname, tregname, entry, fname) \
	(((entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] & \
			FWD_TABLE_FIELD_MASK(tname, tregname, fname)) >> \
			FWD_TABLE_FIELD_SHIFT(tname, tregname, fname))

#define FWD_TABLE_FIELD_GET_BIT(tname, tregname, entry, fname) \
	(!!((entry)[FWD_TABLE_FIELD_DIDX(tname, fname)] & \
		FWD_TABLE_FIELD_BIT(tname, tregname, fname)))

#define FWD_TABLE_FIELD_GET_HELPER1(tname, tregname, entry, fname, ftype) \
	FWD_TABLE_FIELD_GET_##ftype(tname, tregname, entry, fname)
#define FWD_TABLE_FIELD_GET_HELPER2(tname, tregname, entry, fname, ftype) \
	FWD_TABLE_FIELD_GET_HELPER1(tname, tregname, entry, fname, ftype)
#define FWD_TABLE_FIELD_GET(tname, tregname, entry, fname)             \
	FWD_TABLE_FIELD_GET_HELPER2(tname, tregname, entry, fname, \
			FWD_TABLE_FIELD_FTYPE(tname, fname))

/* End of field access macros */

#endif
