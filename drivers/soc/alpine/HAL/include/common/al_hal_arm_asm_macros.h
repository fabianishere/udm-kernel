/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 * @defgroup group_arm_asm_macros HAL ARM assembly macros
 * Includes ARM assembly macros
 *  @{
 * @file   al_hal_arm_asm_macros.h
 *
 */

#ifndef __AL_HAL_ARM_ASM_MACROS_H__
#define __AL_HAL_ARM_ASM_MACROS_H__

#define AL_ARM_ASM_ENTRY(name) \
  .globl name; \
  name:

#define AL_ARM_ASM_END(name) \
  .size name, .-name

#define AL_ARM_ASM_ENDPROC(name) \
  .type name, %function; \
  AL_ARM_ASM_END(name)

/** @} */
#endif

