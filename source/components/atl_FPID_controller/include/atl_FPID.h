/********************************************************************************
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. 
 *    ---------------------------------------------------------------------------                          
 *                      
 *    This software is the property of "Atlas IoT Co. Ltd and may 
 *    not be copied or reproduced otherwise than on to a single hard disk for 
 *    backup or archival purposes.  The source code is confidential  
 *    information and must not be disclosed to third parties or used without  
 *    the express written permission of Atlas IoT Co. Ltd.
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. All rights reserved. 
 *    ----------------------------------------------------------------------------
 *    ----------------------------------------------------------------------------
 *    FILE DESCRIPTION
 *    ----------------------------------------------------------------------------
 *            @file:    atl_FPID.h  
 *          @author:    Patrice Gama
 *            @date:    2022-02-21
 *       @Component:    atl_FPID_controller
 *              
 *     @Description:    The atl Robust proportional integral derivative (RPID) 
 *    control algorithm is an upgrade from the previous traditonal PID controller 
 *    designed. Find detail reference https://ieeexplore.ieee.org/document/1455139 
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#ifndef ROBUST_PID_H
#define ROBUST_PID_H 1

#include <stdint.h>
#include <stddef.h> /* For `NULL`. */

/* A switch to define `ROBUST_PID_FEATURE_VALID_FLT`. */
#if 1
# define ROBUST_PID_FEATURE_VALID_FLT 1 /* To check against floating-point errors. */
/* For `isfinite(), isnan()` (implementation defined). */

# include <math.h>
#endif

/* API and behavior semantic versioning. */
#define ROBUST_PID_LIB_VERSION "1.1.2"

/* This is to help with data-type (Integer, float, double) modification. */
#define ROBUST_PID_FP_ZERO 0.0f
#define ROBUST_PID_FP_ONE 1.0f

/* this is For errors management; Type: ROBUST_PID_info_t */
#define ROBUST_PID_ERR_INIT (0U) /* Bad Initialization. */
#define ROBUST_PID_ERR_FLT (1U)  /* Floating-point error. */
#define ROBUST_PID_ERR_NONE (2U) /* No error detected. */

typedef uint_fast8_t robust_PID_info_t; /* An unsigned type for errors flag. */

/**
 *  This optimizer allgorthm will enable acurate desired temperature control respective to user set-temp 
 */
uint32_t actual_temp_quantification (uint32_t actualtemp, uint32_t settemp);
/**
 *  Set the desired value constrain (the desired value range)
 */
uint32_t Constrain(uint32_t au32_IN, uint32_t au32_MIN, uint32_t au32_MAX);

/**
 *  Transform a value range from a certain value range to another 
 */
uint32_t MAP(uint32_t au32_IN, uint32_t au32_INmin, uint32_t au32_INmax, uint32_t au32_OUTmin, uint32_t au32_OUTmax);

typedef struct {
    /* Controller settings. */
    float kp; /* Gain constant */
    float ki; /* Gain constant */
    float kd; /* Gain constant */

    /* Controller states. */
    float xk_1; /* Physical measurement */
    float xk_2; /* Physical measurement  */

    /* Controller outputs. */
    float p_term; /* The P-term calculated*/
    float i_term; /* The I-term calculated*/
    float d_term; /* The D-term calculated*/

    float error2;

    float y_out; 
} robust_PID_t;

typedef struct {
    float smoothing_factor; 
    float y; 
} robust_PID_lpf_t;

/**
 * Initialized or reset a `robust_PID_t` context by direct gains assignment,
**/
robust_PID_info_t robust_PID_init(robust_PID_t *ctx,
                      float xk_1, float xk_2, float y_previous,
                      float kp, float ki, float kd);
/**
 * Initialized or reset a `robust_PID_t` context by `Kp` gain and time constants `Ti` and `Td`,
**/
robust_PID_info_t robust_PID_init_T(robust_PID_t *ctx,
                        float xk_1, float xk_2, float y_previous,
                        float kp, float ti, float td,
                        float sample_period);

/**
 * This processing is done as a robust PID controller to calculated and update
**/
void robust_pid_calc(robust_PID_t *ctx, float setpoint, float measure);

/**
 * ctx: Pointer to the `robust_PID_t` context.
 * out_min: Min output from controller.
 * out_max: Max output from controller.
**/
void robust_pid_sum(robust_PID_t *ctx, float out_min, float out_max);

/**
 * Initialize or reset a `robust_PID_lpf_t`smoothing_factor.
 * ctx: Pointer to the `robust_PID_lpf_t` context.
 * smoothing_factor: Filter's smoothing factor. `0 < a < 1`.
**/
robust_PID_info_t robust_PID_util_lpf_init(robust_PID_lpf_t *ctx, float smoothing_factor, float x_0);

/**
 * Infinite-impulse-response (IIR) single-pole low-pass filter (LPF),
 * Use this function after `robust_PID_pid_calc()` to apply a D-term low-pass filter.
**/
void robust_PID_util_lpf_calc(robust_PID_lpf_t *ctx, float input);

#endif /* robust_PID_H */
