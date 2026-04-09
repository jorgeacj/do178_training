/*
 * pwm_servo.c
 *
 *  Created on: Apr 2, 2025
 *      Author: reina
 */

#include "pwm_servo.h"

HAL_StatusTypeDef Servo_PWM_Start(Servo_HandleTypeDef *servo)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ((servo != NULL) && (servo->htim != NULL))
    {
        if (HAL_TIM_PWM_Start(servo->htim, servo->channel) == HAL_OK)
        {
            status = HAL_OK;
        }
    }

    return status;
}

HAL_StatusTypeDef Servo_SetAngle(Servo_HandleTypeDef *servo, int32_t angle)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ((servo != NULL) && (servo->htim != NULL))
    {
        /* Clamp the angle between 0° and 180° */
        if (angle < SERVO_ANGLE_MIN)
        {
            angle = SERVO_ANGLE_MIN;
        }
        else if (angle > SERVO_ANGLE_MAX)
        {
            angle = SERVO_ANGLE_MAX;
        }
        else
        {
            /* angle already in range */
        }

        /* Map angle to pulse width: pulse = minPulse + (maxPulse - minPulse) * angle / 180 */
        {
            uint32_t range;
            uint32_t pulse;

            range = servo->maxPulse - servo->minPulse;
            pulse = servo->minPulse + (uint32_t)((range * (uint32_t)angle) / SERVO_ANGLE_RANGE);

            /* Ensure pulse does not exceed the timer period */
            if (pulse > servo->htim->Init.Period)
            {
                pulse = servo->htim->Init.Period;
            }
            else
            {
                /* pulse within range */
            }

            __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse);
        }

        status = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef Servo_SetVal(Servo_HandleTypeDef *servo, int32_t val)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ((servo != NULL) && (servo->htim != NULL))
    {
        /* Clamp the value to the range [0, 100] */
        if (val < SERVO_VAL_MIN)
        {
            val = SERVO_VAL_MIN;
        }
        else if (val > SERVO_VAL_MAX)
        {
            val = SERVO_VAL_MAX;
        }
        else
        {
            /* val already in range */
        }

        /* Map [0, 100] to pulse width: pulse = minPulse + (maxPulse - minPulse) * val / 100 */
        {
            uint32_t range;
            uint32_t pulse;

            range = servo->maxPulse - servo->minPulse;
            pulse = servo->minPulse + (uint32_t)((range * (uint32_t)val) / SERVO_VAL_RANGE);

            /* Ensure pulse does not exceed the timer period */
            if (pulse > servo->htim->Init.Period)
            {
                pulse = servo->htim->Init.Period;
            }
            else
            {
                /* pulse within range */
            }

            __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse);
        }

        status = HAL_OK;
    }

    return status;
}

int32_t Servo_StepTowardAngle(Servo_HandleTypeDef *servo,
                              int32_t currentAngle,
                              int32_t targetAngle,
                              int32_t stepSize)
{
    int32_t result = currentAngle;

    if ((servo == NULL) || (servo->htim == NULL) || (stepSize <= 0))
    {
        result = currentAngle;
    }
    else
    {
        /* Determine the direction and step one increment */
        if (targetAngle > currentAngle)
        {
            currentAngle += stepSize;
            if (currentAngle > targetAngle)
            {
                currentAngle = targetAngle;
            }
            else
            {
                /* within range */
            }
        }
        else if (targetAngle < currentAngle)
        {
            currentAngle -= stepSize;
            if (currentAngle < targetAngle)
            {
                currentAngle = targetAngle;
            }
            else
            {
                /* within range */
            }
        }
        else
        {
            /* targetAngle == currentAngle, no change */
        }

        /* Update the servo with the new angle */
        if (currentAngle != targetAngle)
        {
            (void)Servo_SetAngle(servo, currentAngle);
        }
        else
        {
            /* already at target */
        }

        result = currentAngle;
    }

    return result;
}

/* ======================== ADC feedback API ============================== */

HAL_StatusTypeDef Servo_Feedback_Init(Servo_Feedback_HandleTypeDef *fb)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ((fb != NULL) && (fb->hadc != NULL))
    {
        /* Perform offset calibration for single-ended mode */
        status = HAL_ADCEx_Calibration_Start(fb->hadc,
                                             ADC_CALIB_OFFSET,
                                             ADC_SINGLE_ENDED);

        if (status == HAL_OK)
        {
            /* Start the ADC so it is ready for polling conversions */
            status = HAL_ADC_Start(fb->hadc);
        }
        else
        {
            /* calibration failed – propagate error */
        }
    }
    else
    {
        /* NULL pointer – return HAL_ERROR */
    }

    return status;
}

HAL_StatusTypeDef Servo_Feedback_Read(Servo_Feedback_HandleTypeDef *fb)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ((fb != NULL) && (fb->hadc != NULL))
    {
        /* Trigger a new conversion */
        status = HAL_ADC_Start(fb->hadc);

        if (status == HAL_OK)
        {
            /* Wait for the conversion to complete */
            status = HAL_ADC_PollForConversion(fb->hadc, fb->pollTimeout);
        }
        else
        {
            /* ADC start failed – propagate error */
        }

        if (status == HAL_OK)
        {
            /* Read the 16-bit result and compute the angle */
            fb->lastRaw   = (uint16_t)HAL_ADC_GetValue(fb->hadc);
            fb->lastAngle = Servo_ComputeAngle(fb, fb->lastRaw);
        }
        else
        {
            /* conversion or start failed – leave previous values untouched */
        }
    }
    else
    {
        /* NULL pointer – return HAL_ERROR */
    }

    return status;
}

float Servo_ComputeAngle(const Servo_Feedback_HandleTypeDef *fb,
                         uint16_t raw_adc)
{
    float angle = 0.0f;

    if (fb != NULL)
    {
        if (raw_adc <= fb->adcMin)
        {
            angle = fb->minAngle;
        }
        else if (raw_adc >= fb->adcMax)
        {
            angle = fb->maxAngle;
        }
        else
        {
            /* Linear interpolation:  y = y0 + (y1 - y0) * (x - x0) / (x1 - x0) */
            float span  = fb->maxAngle - fb->minAngle;
            float denom = (float)(fb->adcMax - fb->adcMin);
            float num   = (float)(raw_adc   - fb->adcMin);

            angle = fb->minAngle + (span * num) / denom;
        }
    }
    else
    {
        /* NULL pointer – return 0.0 */
    }

    return angle;
}
