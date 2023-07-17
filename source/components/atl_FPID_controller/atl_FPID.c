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
 *       @Component:    atl_RPID_controller
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

#include "atl_FPID.h"

uint32_t actual_temp_quantification (uint32_t actualtemp, uint32_t settemp){
static uint32_t filter_out;
float average_rate_change = ((float)actualtemp+(float)settemp)/2;                   /*First optimizer*/
//average_rate_change = (average_rate_change+(float)settemp)/2;                     /*Second optimizer*/
if (average_rate_change<settemp){filter_out=ceil(average_rate_change);}
if (average_rate_change>settemp){filter_out=floor(average_rate_change);}
return filter_out; 
}

uint32_t Constrain(uint32_t au32_IN, uint32_t au32_MIN, uint32_t au32_MAX)
{
    if(au32_IN < au32_MIN)
    { return au32_MIN;}
    else if (au32_IN > au32_MAX)
    { return au32_MAX;}
    else
    {return au32_IN;}
}

uint32_t MAP(uint32_t au32_IN, uint32_t au32_INmin, uint32_t au32_INmax, uint32_t au32_OUTmin, uint32_t au32_OUTmax)
{
    return ((((au32_IN - au32_INmin)*(au32_OUTmax - au32_OUTmin))/(au32_INmax - au32_INmin)) + au32_OUTmin);
}

robust_PID_info_t robust_PID_init(robust_PID_t *ctx,
                      float xk_1, float xk_2, float y_previous,
                      float kp, float ki, float kd)
{

#ifdef ROBUST_PID_FEATURE_VALID_FLT
    if ((isfinite(xk_1) == 0)
     || (isfinite(xk_2) == 0)
     || (isfinite(y_previous) == 0)
     || (isfinite(kp) == 0)
     || (isfinite(ki) == 0)
     || (isfinite(kd) == 0)          /*zero can also be apply in case of chosing a PI controller*/
    ) {
        return ROBUST_PID_ERR_FLT;
    }
#endif
    if ((ctx == NULL)
     || (kp <= ROBUST_PID_FP_ZERO)
     || (ki <= ROBUST_PID_FP_ZERO)
     || (kd < ROBUST_PID_FP_ZERO)
    ) {
        return ROBUST_PID_ERR_INIT;
    }

    /* Set previous states for equations. */
    ctx->xk_1 = xk_1;                
    ctx->xk_2 = xk_2;                /*  D-term */
    ctx->y_out = y_previous;         

    /* Direct gains assignments. */
    ctx->kp = kp; /* P-term gain constant. */
    ctx->ki = ki; /* I-term gain constant. */
    ctx->kd = kd; /* D-term gain constant. */

    return ROBUST_PID_ERR_NONE;
}

robust_PID_info_t robust_PID_init_T(robust_PID_t *ctx,
                        float xk_1, float xk_2, float y_previous,
                        float kp, float ti, float td,
                        float sample_period)
{

#ifdef ROBUST_PID_FEATURE_VALID_FLT
    if ((isfinite(ti) == 0)
     || (isfinite(sample_period) == 0)
    ) {
        return ROBUST_PID_ERR_FLT;
    }
#endif

    if ((ti <= ROBUST_PID_FP_ZERO)
     || (td <  ROBUST_PID_FP_ZERO) 
     || (sample_period <= ROBUST_PID_FP_ZERO)
    ) {
        return ROBUST_PID_ERR_INIT;
    }
    
    /* I-term gain constant */
    const float ki = (kp * sample_period) / ti;
    /* D-term gain constant */
    const float kd = kp * (td / sample_period);

    return robust_PID_init(ctx,
                     xk_1, xk_2, y_previous,
                     kp, ki, kd);
}

/*Direct acting RPID controller for fan direction*/

void robust_pid_calc(robust_PID_t *ctx, float setpoint, float measure)
{
    ctx->p_term = ctx->xk_1 - measure;
    ctx->d_term = ctx->kd * (ctx->xk_1 + ctx->p_term - ctx->xk_2);
    ctx->p_term = ctx->kp * ctx->p_term;
    ctx->i_term = ctx->ki * (setpoint - measure);

    ctx->xk_2 = ctx->xk_1;
    ctx->xk_1 = measure;  
    /*error calculate for reverse fan speed*/
    ctx->error2=setpoint-measure;
}

/*Reverse acting RPID controller for fan speed direction*/

// void robust_pid_calc(robust_PID_t *ctx, float setpoint, float measure)
// {
//     /* P-term value: `P[k] = Kp * (x[k-1] - x[k])`
//      * I-term value: `I[k] = Ki * e[k] = Ki * (SP - x[k])`
//      * D-term value: `D[k] = Kp * (2*x[k-1] - x[k-2] - x[k])`
//     */

//     ctx->p_term = ctx->xk_1 - measure;
//     ctx->d_term = ctx->kd * (ctx->xk_1 + ctx->p_term - ctx->xk_2);
//     ctx->p_term = ctx->kp * ctx->p_term;
//     ctx->i_term = ctx->ki * (measure - setpoint);

//     ctx->xk_2 = ctx->xk_1; /* `x[k-2] = x[k-1]` */
//     ctx->xk_1 = measure;   /* `x[k-1] = x[k]` */

//     // error calculate for forward fan speed 
//     ctx->error2=measure-setpoint;
// }

void robust_pid_sum(robust_PID_t *ctx, float out_min, float out_max)
{
#ifdef ROBBUST_PID_FEATURE_VALID_FLT
    const float y_prev = ctx->y_out;
#endif
    ctx->y_out += ctx->p_term + ctx->i_term + ctx->d_term;

#ifdef ROBBUST_PID_FEATURE_VALID_FLT
    if ((isnan(ctx->y_out) != 0)
     || (isnan(ctx->p_term) != 0)
     || (isnan(ctx->i_term) != 0)
     || (isnan(ctx->d_term) != 0)
    ) {
        ctx->y_out = y_prev;
    }
#endif

    /* Limit the new output (CV) to boundaries. */
    if (ctx->y_out > out_max) {
        ctx->y_out = out_max;
    }
    else if (ctx->y_out < out_min) {
        ctx->y_out = out_min;
    }
}

robust_PID_info_t robust_PID_util_lpf_init(robust_PID_lpf_t *ctx, float smoothing_factor, float x_0)
{
#ifdef ROBUST_PID_FEATURE_VALID_FLT
    if ((isfinite(smoothing_factor) == 0)
     || (isfinite(x_0) == 0)
    ) {
        return ROBUST_PID_ERR_FLT;
    }
#endif
    if ((ctx == NULL)
     || (smoothing_factor <= ROBUST_PID_FP_ZERO)
     || (smoothing_factor >= ROBUST_PID_FP_ONE)
    ) {
        return ROBUST_PID_ERR_INIT;
    }
    /* Filter's smoothing factor. `0 < a < 1` */
    ctx->smoothing_factor = smoothing_factor;
    /* `y = smoothing_factor  */
    ctx->y = smoothing_factor * x_0;
    return ROBUST_PID_ERR_NONE;
}

void robust_PID_util_lpf_calc(robust_PID_lpf_t *ctx, float input)
{
    /* Infinite-impulse-response (IIR) single-pole low-pass filter,
     * an exponentially weighted moving average (EMA).
    */

    const float y_prev = ctx->y;
    ctx->y = y_prev + ctx->smoothing_factor * (input - y_prev);
}

/*Fuzzy logic implementation*/

