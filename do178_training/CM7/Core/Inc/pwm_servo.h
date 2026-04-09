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

/* Pulse width range (timer counts = µs when 1 count = 1 µs) */
#define SERVO_MIN_PULSE    530U
#define SERVO_MAX_PULSE    2400U

/* ---------- ADC feedback calibration defaults (16-bit ADC) ---------- */
#define SERVO_FB_ADC_MIN       2435U    /**< ADC count at 0°   (from linear fit) */
#define SERVO_FB_ADC_MAX       57996U   /**< ADC count at 180° (from linear fit) */
#define SERVO_FB_ANGLE_MIN     0.0f     /**< Angle at ADC minimum [deg]          */
#define SERVO_FB_ANGLE_MAX     180.0f   /**< Angle at ADC maximum [deg]          */
#define SERVO_FB_POLL_TIMEOUT  10U      /**< Default poll timeout [ms]           */

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
  * @brief  Servo_Feedback_HandleTypeDef
  * Encapsulates the ADC peripheral used for analog position feedback,
  * together with calibration endpoints and the most recent reading.
  */
typedef struct
{
    ADC_HandleTypeDef *hadc;      /**< Pointer to the ADC handle (e.g., &hadc1)  */
    uint32_t          pollTimeout;/**< HAL_ADC_PollForConversion timeout [ms]    */
    uint16_t          adcMin;     /**< ADC count corresponding to minAngle       */
    uint16_t          adcMax;     /**< ADC count corresponding to maxAngle       */
    float             minAngle;   /**< Physical angle at adcMin [deg]            */
    float             maxAngle;   /**< Physical angle at adcMax [deg]            */
    uint16_t          lastRaw;    /**< Most recent raw ADC reading (output)      */
    float             lastAngle;  /**< Most recent computed angle [deg] (output) */
} Servo_Feedback_HandleTypeDef;

/* ========================= PWM output API ============================== */

/**
  * @brief  Starts the PWM signal on the configured timer channel.
  * @param  servo: pointer to a Servo_HandleTypeDef structure (shall not be NULL).
  * @retval HAL_OK on success, HAL_ERROR on failure or invalid parameter.
  */
HAL_StatusTypeDef Servo_PWM_Start(Servo_HandleTypeDef *servo);

/**
  * @brief  Sets the servo to a specified angle between 0° and 180°.
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

/* ===================== ADC feedback API ================================ */

/**
  * @brief  Initialises the ADC feedback subsystem.
  *
  *         Performs offset calibration (single-ended) and starts the ADC
  *         so it is ready for subsequent polling reads.
  *
  * @param  fb  Pointer to a fully-populated Servo_Feedback_HandleTypeDef
  *             (shall not be NULL; fb->hadc shall not be NULL).
  * @retval HAL_OK on success, HAL_ERROR on invalid parameter or HAL failure.
  */
HAL_StatusTypeDef Servo_Feedback_Init(Servo_Feedback_HandleTypeDef *fb);

/**
  * @brief  Triggers one ADC conversion, waits for completion, and computes
  *         the servo feedback angle.
  *
  *         On success the results are stored in fb->lastRaw and fb->lastAngle.
  *
  * @param  fb  Pointer to a Servo_Feedback_HandleTypeDef that has been
  *             initialised with Servo_Feedback_Init (shall not be NULL).
  * @retval HAL_OK on success, HAL_ERROR on invalid parameter or conversion failure.
  */
HAL_StatusTypeDef Servo_Feedback_Read(Servo_Feedback_HandleTypeDef *fb);

/**
  * @brief  Pure computation: converts a raw 16-bit ADC value to an angle
  *         using the calibration parameters stored in the feedback handle.
  *
  * @param  fb       Pointer to a const Servo_Feedback_HandleTypeDef
  *                  (shall not be NULL).
  * @param  raw_adc  Raw 16-bit ADC reading.
  * @retval Computed angle in degrees, clamped to [minAngle, maxAngle].
  */
float Servo_ComputeAngle(const Servo_Feedback_HandleTypeDef *fb,
                         uint16_t raw_adc);

#endif /* INC_PWM_SERVO_H_ */
