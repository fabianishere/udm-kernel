/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __AL_HAL_IOFIC_REG_H
#define __AL_HAL_IOFIC_REG_H

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/

struct al_iofic_grp_ctrl {
	/*
	 * [0x0] Interrupt Cause Register
	 * Set by hardware
	 * - If MSI-X is enabled, and auto_clear control bit =TRUE, automatically cleared after
	 * MSI-X message associated with this specific interrupt bit is sent (MSI-X acknowledge is
	 * received).
	 * - Software may set a bit in this register by writing 1 to the associated bit in the
	 * Interrupt Cause Set register
	 * Write-0 clears a bit. Write-1 has no effect.
	 * - On CPU Read, if clear_on_read control bit =TRUE, automatically cleared (all bits are
	 * cleared).
	 * When there is a conflict, and on the same clock cycle hardware tries to set a bit in the
	 * Interrupt Cause register, the specific bit is set to ensure that the interrupt indication
	 * is not lost.
	 */
	uint32_t int_cause_grp;
	uint32_t rsrvd1;
	/*
	 * [0x8] Interrupt Cause Set Register
	 * Writing 1 to a bit in this register sets its corresponding cause bit, enabling software
	 * to generate a hardware interrupt. Write 0 has no effect.
	 */
	uint32_t int_cause_set_grp;
	uint32_t rsrvd2;
	/*
	 * [0x10] Interrupt Mask Register
	 * If Auto-mask control bit =TRUE, automatically set to 1 after MSI-X message associated
	 * with the associated interrupt bit is sent. (AXI write acknowledge is received.)
	 */
	uint32_t int_mask_grp;
	uint32_t rsrvd3;
	/*
	 * [0x18] Interrupt Mask Clear Register
	 * Used when auto-mask control bit = True. Enables CPU to clear specific bit. It prevents a
	 * scenario in which the CPU overrides another bit with 1 (old value) that hardware has just
	 * cleared to 0.
	 * Write 0 to this register clears its corresponding mask bit. Write 1 has no effect
	 */
	uint32_t int_mask_clear_grp;
	uint32_t rsrvd4;
	/*
	 * [0x20] Interrupt Status Register
	 * This register latches the status of the interrupt source.
	 */
	uint32_t int_status_grp;
	uint32_t rsrvd5;
	/* [0x28] Interrupt Control Register */
	uint32_t int_control_grp;
	uint32_t rsrvd6;
	/*
	 * [0x30] Interrupt Mask Register
	 * Each bit in this register masks the corresponding cause bit for generating an Abort
	 * signal. Its default value is determined by unit instantiation.
	 * Abort = Wire-OR of Cause & !Interrupt_Abort_Mask).
	 * This register provides error handling configuration for error interrupts.
	 */
	uint32_t int_abort_msk_grp;
	uint32_t rsrvd7;
	/*
	 * [0x38] Interrupt Log Register
	 * Each bit in this register masks the corresponding cause bit for capturing the log
	 * registers. Its default value is determined by unit instantiation.
	 * Log_capture = Wire-OR of Cause & !Interrupt_Log_Mask).
	 * This register provides error handling configuration for error interrupts.
	 */
	uint32_t int_log_msk_grp;
	uint32_t rsrvd8;
};

struct al_iofic_grp_mod {
	/*
	 * [0x0] Interrupt Moderation Register
	 * Dedicated moderation interval per bit.
	 */
	uint32_t grp_int_mod_reg;
	/* [0x4] Target ID */
	uint32_t grp_int_tgtid_reg;
};

struct al_iofic_regs {
	struct al_iofic_grp_ctrl ctrl[0];
	uint32_t rsrvd1[0x400 >> 2];
	struct al_iofic_grp_mod grp_int_mod[0][32];
};


/*
* Registers Fields
*/


/**** int_control_grp register ****/
/* When Clear_on_Read =1, all bits of Cause register are cleared on read. */
#define INT_CONTROL_GRP_CLEAR_ON_READ (1 << 0)
/*
 * (Must be set only when MSIX is enabled.)
 * When Auto-Mask =1 and an MSI-X ACK for this bit is received, its corresponding bit in the mask
 * register is set, masking future interrupt.s
 */
#define INT_CONTROL_GRP_AUTO_MASK (1 << 1)
/*
 * Auto_Clear (RW)
 * When Auto-Clear =1, the bits in the Interrupt Cause register are auto-cleared after MSI-X is
 * acknowledged. Must be used only if MSI-X is enabled.
 */
#define INT_CONTROL_GRP_AUTO_CLEAR (1 << 2)
/*
 * When Set_on_Posedge =1, the bits in the Interrupt Cause register are set on the posedge of the
 * interrupt source, i.e., when interrupt source =1 and Interrupt Status = 0.
 * When Set_on_Posedge =0, the bits in the Interrupt Cause register are set when interrupt source
 * =1.
 */
#define INT_CONTROL_GRP_SET_ON_POSEDGE (1 << 3)
/*
 * When Moderation_Reset =1, all Moderation timers associated with the interrupt cause bits are
 * cleared to 0, enabling immediate interrupt assertion if any unmasked cause bit is set to 1. This
 * bit is self-negated.
 */
#define INT_CONTROL_GRP_MOD_RST (1 << 4)
/*
 * When mask_msi_x =1, no MSI-X from this group is sent. This bit must be set to 1 when the
 * associated summary bit in this group is used to generate a single MSI-X for this group.
 */
#define INT_CONTROL_GRP_MASK_MSI_X (1 << 5)
/*
 * Asserting this bit results in MSIX Address[63:20] sharing by the whole group. Default is per
 * entry.
 */
#define INT_CONTROL_GRP_ADDR_HI_COMPAT_MODE (1 << 7)
/* MSI-X AWID value, same ID for all cause bits. */
#define INT_CONTROL_GRP_AWID_MASK 0x00000F00
#define INT_CONTROL_GRP_AWID_SHIFT 8
/* This value determines the interval between interrupts. Writing ZERO disables Moderation. */
#define INT_CONTROL_GRP_MOD_INTV_MASK 0x00FF0000
#define INT_CONTROL_GRP_MOD_INTV_SHIFT 16
/*
 * The resolution value determines the Moderation_Timer_Clock speed.
 * 0- Moderation-timer is decremented every 1x256 SB clock cycles ~1uS.
 * 1- Moderation-timer is decremented every 2x256 SB clock cycles ~2uS.
 * N- Moderation-timer is decremented every Nx256 SB clock cycles ~(N+1) uS.
 * When timer resolution is cleared to zero, the coalescing-interval must also be cleared to zero
 */
#define INT_CONTROL_GRP_MOD_RES_MASK 0x0F000000
#define INT_CONTROL_GRP_MOD_RES_SHIFT 24

/**** grp0_int_mod_reg register ****/
/*
 * Interrupt Moderation Interval Register
 * A dedicated register is allocated per bit in the group. This value determines the interval
 * between interrupts for the associated bit in the group. Writing ZERO disables Moderation.
 */
#define INT_MOD_INTV_MASK 0x000000FF
#define INT_MOD_INTV_SHIFT 0

/**** grp_int_tgtid_reg register ****/
/* Target ID field. */
#define INT_MSIX_TGTID_MASK 0x0000FFFF
#define INT_MSIX_TGTID_SHIFT 0
/* Target ID enable. */
#define INT_MSIX_TGTID_EN_SHIFT 31

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_IOFIC_REG_H */




