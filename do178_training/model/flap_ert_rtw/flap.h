/*
 * File: flap.h
 *
 * Code generated for Simulink model 'flap'.
 *
 * Model version                  : 1.30
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Tue Mar 24 10:57:54 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. Traceability
 * Validation result: Not run
 */

#ifndef RTW_HEADER_flap_h_
#define RTW_HEADER_flap_h_
#ifndef flap_COMMON_INCLUDES_
#define flap_COMMON_INCLUDES_
#include <stdbool.h>
#include <stdint.h>
#endif                                 /* flap_COMMON_INCLUDES_ */

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  double Filter_DSTATE;                /* '<S29>/Filter' */
  double Integrator_DSTATE;            /* '<S34>/Integrator' */
  uint8_t is_active_c3_flap;           /* '<Root>/Chart' */
  uint8_t is_c3_flap;                  /* '<Root>/Chart' */
  uint8_t temporalCounter_i1;          /* '<Root>/Chart' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  double FeedbackServo;                /* '<Root>/FeedbackServo' */
  double Button;                       /* '<Root>/Button' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  double angle;                        /* '<Root>/angle' */
  double error;                        /* '<Root>/error' */
  double control;                      /* '<Root>/control' */
  double ctrl_sat;                     /* '<Root>/ctrl_sat' */
} ExtY;

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void flap_initialize(void);
extern void flap_step(void);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<Root>/Scope' : Unused code path elimination
 * Block '<S31>/Integral Gain' : Eliminated nontunable gain of 1
 * Block '<S39>/Proportional Gain' : Eliminated nontunable gain of 1
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'flap'
 * '<S1>'   : 'flap/C'
 * '<S2>'   : 'flap/Chart'
 * '<S3>'   : 'flap/C/Anti-windup'
 * '<S4>'   : 'flap/C/D Gain'
 * '<S5>'   : 'flap/C/Filter'
 * '<S6>'   : 'flap/C/Filter ICs'
 * '<S7>'   : 'flap/C/I Gain'
 * '<S8>'   : 'flap/C/Ideal P Gain'
 * '<S9>'   : 'flap/C/Ideal P Gain Fdbk'
 * '<S10>'  : 'flap/C/Integrator'
 * '<S11>'  : 'flap/C/Integrator ICs'
 * '<S12>'  : 'flap/C/N Copy'
 * '<S13>'  : 'flap/C/N Gain'
 * '<S14>'  : 'flap/C/P Copy'
 * '<S15>'  : 'flap/C/Parallel P Gain'
 * '<S16>'  : 'flap/C/Reset Signal'
 * '<S17>'  : 'flap/C/Saturation'
 * '<S18>'  : 'flap/C/Saturation Fdbk'
 * '<S19>'  : 'flap/C/Sum'
 * '<S20>'  : 'flap/C/Sum Fdbk'
 * '<S21>'  : 'flap/C/Tracking Mode'
 * '<S22>'  : 'flap/C/Tracking Mode Sum'
 * '<S23>'  : 'flap/C/Tsamp - Integral'
 * '<S24>'  : 'flap/C/Tsamp - Ngain'
 * '<S25>'  : 'flap/C/postSat Signal'
 * '<S26>'  : 'flap/C/preSat Signal'
 * '<S27>'  : 'flap/C/Anti-windup/Passthrough'
 * '<S28>'  : 'flap/C/D Gain/Internal Parameters'
 * '<S29>'  : 'flap/C/Filter/Disc. Forward Euler Filter'
 * '<S30>'  : 'flap/C/Filter ICs/Internal IC - Filter'
 * '<S31>'  : 'flap/C/I Gain/Internal Parameters'
 * '<S32>'  : 'flap/C/Ideal P Gain/Passthrough'
 * '<S33>'  : 'flap/C/Ideal P Gain Fdbk/Disabled'
 * '<S34>'  : 'flap/C/Integrator/Discrete'
 * '<S35>'  : 'flap/C/Integrator ICs/Internal IC'
 * '<S36>'  : 'flap/C/N Copy/Disabled'
 * '<S37>'  : 'flap/C/N Gain/Internal Parameters'
 * '<S38>'  : 'flap/C/P Copy/Disabled'
 * '<S39>'  : 'flap/C/Parallel P Gain/Internal Parameters'
 * '<S40>'  : 'flap/C/Reset Signal/Disabled'
 * '<S41>'  : 'flap/C/Saturation/Passthrough'
 * '<S42>'  : 'flap/C/Saturation Fdbk/Disabled'
 * '<S43>'  : 'flap/C/Sum/Sum_PID'
 * '<S44>'  : 'flap/C/Sum Fdbk/Disabled'
 * '<S45>'  : 'flap/C/Tracking Mode/Disabled'
 * '<S46>'  : 'flap/C/Tracking Mode Sum/Passthrough'
 * '<S47>'  : 'flap/C/Tsamp - Integral/TsSignalSpecification'
 * '<S48>'  : 'flap/C/Tsamp - Ngain/Passthrough'
 * '<S49>'  : 'flap/C/postSat Signal/Forward_Path'
 * '<S50>'  : 'flap/C/preSat Signal/Forward_Path'
 */

/*-
 * Requirements for '<Root>': flap

 */
#endif                                 /* RTW_HEADER_flap_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
