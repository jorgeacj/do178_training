/*
 * File: flap.c
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

#include "flap.h"
#include <stdint.h>

/* Named constants for Chart: '<Root>/Chart' */
#define IN_Cruise                      ((uint8_t)1U)
#define IN_Idle                        ((uint8_t)2U)
#define IN_Landing                     ((uint8_t)3U)
#define IN_Take_off                    ((uint8_t)4U)
#define IN_Taxing                      ((uint8_t)5U)

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Model step function */
void flap_step(void)
{
	double rtb_FilterCoefficient;
	int32_t rtb_angle;

	/* Chart: '<Root>/Chart' incorporates:
	 *  Inport: '<Root>/Button'
	 */
	if (rtDW.temporalCounter_i1 < 63U) {
		rtDW.temporalCounter_i1++;
	}

	if (rtDW.is_active_c3_flap == 0U) {
		rtDW.is_active_c3_flap = 1U;
		rtDW.is_c3_flap = IN_Idle;
		rtb_angle = 0;
	} else {
		switch (rtDW.is_c3_flap) {
		case IN_Cruise:
			rtb_angle = 30;
			if (rtDW.temporalCounter_i1 >= 40U) {
				rtDW.is_c3_flap = IN_Landing;
				rtb_angle = 90;
			}
			break;

		case IN_Idle:
			rtb_angle = 0;
			if (rtU.Button == 1.0) {
				rtDW.is_c3_flap = IN_Taxing;
				rtDW.temporalCounter_i1 = 0U;
			}
			break;

		case IN_Landing:
			rtb_angle = 90;
			break;

		case IN_Take_off:
			rtb_angle = 90;
			if (rtDW.temporalCounter_i1 >= 20U) {
				rtDW.is_c3_flap = IN_Cruise;
				rtDW.temporalCounter_i1 = 0U;
				rtb_angle = 30;
			}
			break;

		default:
			/* case IN_Taxing: */
			rtb_angle = 0;
			if (rtDW.temporalCounter_i1 >= 10U) {
				rtDW.is_c3_flap = IN_Take_off;
				rtDW.temporalCounter_i1 = 0U;
				rtb_angle = 90;
			}
			break;
		}
	}

	/* End of Chart: '<Root>/Chart' */

	/* Outport: '<Root>/angle' */
	rtY.angle = rtb_angle;

	/* Sum: '<Root>/Sum' incorporates:
	 *  Inport: '<Root>/FeedbackServo'
	 *  Outport: '<Root>/angle'
	 */
	rtY.error = (double)rtb_angle - rtU.FeedbackServo;

	/* Gain: '<S37>/Filter Coefficient' incorporates:
	 *  DiscreteIntegrator: '<S29>/Filter'
	 *  Gain: '<S28>/Derivative Gain'
	 *  Sum: '<S29>/SumD'
	 */
	rtb_FilterCoefficient = (0.1 * rtY.error - rtDW.Filter_DSTATE) * 100.0;

	/* Sum: '<S43>/Sum' incorporates:
	 *  DiscreteIntegrator: '<S34>/Integrator'
	 */
	rtY.control = (rtY.error + rtDW.Integrator_DSTATE) + rtb_FilterCoefficient;

	/* Saturate: '<Root>/Saturation' */
	if (rtY.control > 1.0) {
		/* Outport: '<Root>/ctrl_sat' */
		rtY.ctrl_sat = 1.0;
	} else if (rtY.control < 0.0) {
		/* Outport: '<Root>/ctrl_sat' */
		rtY.ctrl_sat = 0.0;
	} else {
		/* Outport: '<Root>/ctrl_sat' */
		rtY.ctrl_sat = rtY.control;
	}

	/* End of Saturate: '<Root>/Saturation' */

	/* Update for DiscreteIntegrator: '<S29>/Filter' */
	rtDW.Filter_DSTATE += 0.1 * rtb_FilterCoefficient;

	/* Update for DiscreteIntegrator: '<S34>/Integrator' */
	rtDW.Integrator_DSTATE += 0.1 * rtY.error;
}

/* Model initialize function */
void flap_initialize(void)
{
	/* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
