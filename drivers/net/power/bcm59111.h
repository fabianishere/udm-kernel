#ifndef _BCM59111_H__
#define _BCM59111_H__

/* Interrupt Registers */
#define BCM59111_INT_REG                0x00
#define BCM59111_INT_MASK_REG           0x01

/* Interrupt flags */
#define BCM59111_INT_F_PWR_EN           0x1
#define BCM59111_INT_F_PWR_GOOD         0x2
#define BCM59111_INT_F_DIS              0x4
#define BCM59111_INT_F_DET              0x8
#define BCM59111_INT_F_CLASS            0x10
#define BCM59111_INT_F_TCUT             0x20
#define BCM59111_INT_F_TSTA             0x40
#define BCM59111_INT_F_SUP              0x80

/* Event Registers */
#define BCM59111_PWR_EVT_REG            0x02
#define BCM59111_PWR_EVT_CLR_REG        0x03

#define BCM59111_DET_CLASS_REG          0x04
#define BCM59111_DET_CLASS_CLR_REG      0x05

#define BCM59111_FAULT_EVT_REG          0x06
#define BCM59111_FAULT_EVT_CLR_REG      0x07

#define BCM59111_TS_EVT_REG             0x08
#define BCM59111_TS_EVT_CLR_REG         0x09

#define BCM59111_SUP_EVT_REG            0x0a
#define BCM59111_SUP_EVT_CLR_REG        0x0b

/* Status Registers */
#define BCM59111_SAT_P1_REG             0x0c
#define BCM59111_SAT_P2_REG             0x0d
#define BCM59111_SAT_P3_REG             0x0e
#define BCM59111_SAT_P4_REG             0x0f

#define BCM59111_SAT_PWR_REG            0x10

/* Power status */
#define BCM59111_PWR_GOOD               1
#define BCM59111_PWR_ENABLE             16
#define BCM59111_IS_PWR_ENABLE(_pwr_status) ((_pwr_status) & BCM59111_PWR_ENABLE)
#define BCM59111_IS_PWR_GOOD(_pwr_status)   ((_pwr_status) & BCM59111_PWR_GOOD)

#define BCM59111_DEV_GET_PORT_PWR_STATUS(_port, _val) \
    (((_val) >> (_port)) & (BCM59111_PWR_GOOD | BCM59111_PWR_ENABLE))

#define BCM59111_SAT_PIN_REG            0x11

/* Config Registers */
#define BCM59111_OP_MOD_REG             0x12

enum bcm59111_reg_op_mod {

    BCM59111_OP_MOD_SHUT    = 0x0,
    BCM59111_OP_MOD_MANU    = 0x1,
    BCM59111_OP_MOD_SEMI    = 0x2,
    BCM59111_OP_MOD_AUTO    = 0x3,
};

#define BCM59111_OP_MOD_MASK            0x03
#define BCM59111_DEV_GET_PORT_OPMODE(_port, _val) \
    (((_val) >> ((_port) << 1)) & BCM59111_OP_MOD_MASK)

#define BCM59111_DEV_SET_PORT_OPMODE(_port, _val_old, _mode) \
    ((((_val_old)&~(BCM59111_OP_MOD_MASK<<((_port)<<1)))|((_mode)<<((_port)<<1)))&0xFF)

#define BCM59111_DIS_SEN_EN_REG         0x13
#define BCM59111_DET_CLASS_EN_REG       0x14
#define BCM59111_MS_BO_EN_REG           0x15
#define BCM59111_MISC_CONF_REG          0x17

/* Pushbuttons Registers */
#define BCM59111_DET_CLASS_PUSH_REG     0x18
#define BCM59111_PWR_ONOFF_PUSH_REG     0x19
#define BCM59111_RST_PUSH_REG           0x1a

/* Debug Registers */
#define BCM59111_ID_REG                 0x1b

/* Limit timing Registers */
#define BCM59111_TIMER_P1P2_REG         0x1e
#define BCM59111_TIMER_P3P4_REG         0x1f

/* Port parametric measuremnet Registers */
#define BCM59111_P1_CM_LSB_REG          0x30
#define BCM59111_P1_CM_MSB_REG          0x31
#define BCM59111_P1_VM_LSB_REG          0x32
#define BCM59111_P1_VM_MSB_REG          0x33

#define BCM59111_P2_CM_LSB_REG          0x34
#define BCM59111_P2_CM_MSB_REG          0x35
#define BCM59111_P2_VM_LSB_REG          0x36
#define BCM59111_P2_VM_MSB_REG          0x37

#define BCM59111_P3_CM_LSB_REG          0x38
#define BCM59111_P3_CM_MSB_REG          0x39
#define BCM59111_P3_VM_LSB_REG          0x3a
#define BCM59111_P3_VM_MSB_REG          0x3b

#define BCM59111_P4_CM_LSB_REG          0x3c
#define BCM59111_P4_CM_MSB_REG          0x3d
#define BCM59111_P4_VM_LSB_REG          0x3e
#define BCM59111_P4_VM_MSB_REG          0x3f

#define BCM59111_PWR_CURRENT_UNIT       122.07  // uA
#define BCM59111_PWR_VOLTAGE_UNIT       5.835   // mV

/* Config1 Registers */
#define BCM59111_FW_REV_REG             0x41
#define BCM59111_WDT_TIME_REG           0x42
#define BCM59111_DEV_ID_REG             0x43

#define BCM59111_DEV_ID_REV             0xC0
#define BCM59121_DEV_ID_REV             0x60

/* High power feature global Registers */
#define BCM59111_HP_EN_REG              0x44

/* High power feature, port 1 Registers */
#define BCM59111_HP_P1_MOD_REG          0x46
#define BCM59111_HP_P1_OV_CUT_REG       0x47
#define BCM59111_HP_P1_CL_FBC_REG       0x48
#define BCM59111_HP_P1_SAT_REG          0x49

/* High power feature, port 2 Registers */
#define BCM59111_HP_P2_MOD_REG          0x4b
#define BCM59111_HP_P2_OV_CUT_REG       0x4c
#define BCM59111_HP_P2_CL_FBC_REG       0x4d
#define BCM59111_HP_P2_SAT_REG          0x4e

/* High power feature, port 3 Registers */
#define BCM59111_HP_P3_MOD_REG          0x50
#define BCM59111_HP_P3_OV_CUT_REG       0x51
#define BCM59111_HP_P3_CL_FBC_REG       0x52
#define BCM59111_HP_P3_SAT_REG          0x53

/* High power feature, port 4 Registers */
#define BCM59111_HP_P4_MOD_REG          0x55
#define BCM59111_HP_P4_OV_CUT_REG       0x56
#define BCM59111_HP_P4_CL_FBC_REG       0x57
#define BCM59111_HP_P4_SAT_REG          0x58

/* Firmware control registers */
#define SEGMENT_SIZE    8
#define BCM59111_HEADER_OFFSET 		3
#define BCM59111_FW_DWN_CTRL 		0x70
#define BCM59111_FW_PROGRAM 		(0x1 << 7)
#define BCM59111_FW_FORCE_CRC 		(0x1 << 6)
#define BCM59111_CRC_STATUS 		0x75

#define I2C_POE_BCAST_SADDR 	0x30

struct reg_init {
    uint8_t reg;
    uint8_t val;
};

#endif
