/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

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

/**
 * @defgroup group_serdes_api API
 * SerDes HAL driver API
 * @ingroup group_serdes
 *
 *  @{
 * @file   al_hal_serdes_hssp_xtra_cfg_pcie3.h
 *
 * @brief  SerDes HAL driver - auto generated core extra configuration for
 *                             PCIE3
 *
 */

static inline int al_serdes_group_cfg_WrReg_PCIE3_Extra_Config_4lane(
	struct al_serdes_grp_obj *grp_obj)
{
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 101, 191);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 102, 191);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 103, 8);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 104, 5);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 105, 19);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 106, 34);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 107, 1);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 108, 1);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 109, 49);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 110, 5);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 101, 170);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 102, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 103, 69);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 104, 191);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 105, 191);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 106, 8);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 107, 5);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 108, 19);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 109, 34);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 110, 5);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 111, 5);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 112, 16);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 113, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 114, 16);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 115, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 116, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 117, 207);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 118, 247);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 119, 225);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 120, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 121, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 122, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 123, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 124, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 125, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 126, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 127, 227);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 128, 231);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 129, 219);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 130, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 131, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 132, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 133, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 134, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 135, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 136, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 137, 227);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 138, 231);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 139, 219);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 140, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 141, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 142, 253);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 143, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 144, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 145, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 146, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 147, 255);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 148, 245);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 149, 63);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 150, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 151, 63);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 152, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 153, 3);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 154, 1);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 155, 1);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 156, 1);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 157, 4);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 158, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 159, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 160, 40);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 161, 4);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 162, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 163, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 164, 4);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0_LANE_0,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_1_LANE_1,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_2_LANE_2,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_3_LANE_3,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 13, 16);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 48, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 49, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 54, 0);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 55, 166);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 93, 2);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 165, 2);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 41, 6);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 354, 3);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 355, 58);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 356, 9);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 357, 3);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 358, 62);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 359, 12);
	al_serdes_grp_reg_write(grp_obj, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 701, 0);

	return 0;
}

/** @} end of SERDES group */
