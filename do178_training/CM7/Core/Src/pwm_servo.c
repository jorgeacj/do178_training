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
        /* Clamp the angle between 0deg and 180deg */
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
