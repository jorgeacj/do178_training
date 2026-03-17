/*
 * pwm_servo.h
 *
 *  Created on: Apr 2, 2025
 *      Author: reina
 */

#ifndef INC_PWM_SERVO_H_
#define INC_PWM_SERVO_H_

#include "stm32h7xx_hal.h"
#include <stddef.h>
#include <stdint.h>

/* Servo angle range (degrees) */
#define SERVO_ANGLE_MIN    0
#define SERVO_ANGLE_MAX    180
#define SERVO_ANGLE_RANGE  180

/* Servo value range (percent) */
#define SERVO_VAL_MIN      0
#define SERVO_VAL_MAX      100
#define SERVO_VAL_RANGE    100

/* Pulse width range (timer counts = us when 1 count = 1 us) */
#define SERVO_MIN_PULSE    530U
#define SERVO_MAX_PULSE    2400U

/**
  * @brief  Servo_HandleTypeDef
  * This structure encapsulates the PWM timer handle, channel, and the pulse widths
  * corresponding to the minimum and maximum output values.
  */
typedef struct
{
    TIM_HandleTypeDef *htim;   /**< Pointer to the timer handle (e.g., &htim2) */
    uint32_t channel;         /**< Timer channel (e.g., TIM_CHANNEL_1) */
    uint32_t minPulse;        /**< Minimum pulse width (must be < Period) */
    uint32_t maxPulse;        /**< Maximum pulse width (must be < Period) */
} Servo_HandleTypeDef;

/**
  * @brief  Starts the PWM signal on the configured timer channel.
  * @param  servo: pointer to a Servo_HandleTypeDef structure (shall not be NULL).
  * @retval HAL_OK on success, HAL_ERROR on failure or invalid parameter.
  */
HAL_StatusTypeDef Servo_PWM_Start(Servo_HandleTypeDef *servo);

/**
  * @brief   Sets the servo to a specified angle between 0 and 180 degrees.
  * @param  servo: pointer to a Servo_HandleTypeDef structure (shall not be NULL).
  * @param  angle: Desired servo angle (in degrees).
  * @retval HAL_OK on success, HAL_ERROR on invalid parameter.
  */
HAL_StatusTypeDef Servo_SetAngle(Servo_HandleTypeDef *servo, int32_t angle);

/**
  * @brief  Sets the servo to a value between 0 and 100 (percent).
  * @param  servo: pointer to a Servo_HandleTypeDef structure (shall not be NULL).
  * @param  val: Desired value (0-100).
  * @retval HAL_OK on success, HAL_ERROR on invalid parameter.
  */
HAL_StatusTypeDef Servo_SetVal(Servo_HandleTypeDef *servo, int32_t val);

/**
  * @brief  Steps the servo one increment toward a target angle.
  * @param  servo: pointer to a Servo_HandleTypeDef structure (shall not be NULL).
  * @param  currentAngle: current servo angle (in degrees).
  * @param  targetAngle: target servo angle (in degrees).
  * @param  stepSize: incremental step size in degrees (must be > 0).
  * @retval Updated current angle, or currentAngle if invalid parameters.
  */
int32_t Servo_StepTowardAngle(Servo_HandleTypeDef *servo,
                              int32_t currentAngle,
                              int32_t targetAngle,
                              int32_t stepSize);

#endif /* INC_PWM_SERVO_H_ */
