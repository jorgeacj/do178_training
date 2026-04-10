# PWM Servo Control Module — Software Design Document

| Field                  | Value           |
| ---------------------- | --------------- |
| **Document ID**        | SDD-CM7-PWM-001 |
| **Version**            | 2.0.0           |
| **Date**               | 2025-04-09      |
| **Author**             |             |
| **Configuration Item** | CI-CM7-PWM      |
| **CSCI**               | CSCI-SERVO-CTRL |
| **DAL**                | C (Major)       |
| **Status**             | Draft           |

---

## 1. Purpose

This document describes the software design, requirements traceability, and
verification plan for the **PWM Servo Control Module** (`pwm_servo.h` /
`pwm_servo.c`), developed in compliance with **DO-178B** and **MISRA C:2012**.

---


### 2. Current Assignment

This module is currently assigned **DAL-C (Major)**, assuming the servo is a
secondary actuator. If the SSA/FHA for your system yields a different failure
condition classification, update the `@dal` tag in both source files accordingly.

---

## 3. Module Architecture

### 3.1 Files

| File                       | Role                   |
| -------------------------- | ---------------------- |
| `CM7/Core/Inc/pwm_servo.h` | Public interface (API) |
| `CM7/Core/Src/pwm_servo.c` | Implementation         |

### 3.2 Data Structures

#### Servo_HandleTypeDef (PWM output)

```c
typedef struct
{
    TIM_HandleTypeDef *htim;   /* Pointer to the timer handle  */
    uint32_t channel;          /* Timer channel                */
    uint32_t minPulse;         /* Minimum pulse width (counts) */
    uint32_t maxPulse;         /* Maximum pulse width (counts) */
} Servo_HandleTypeDef;
```

#### Servo_Feedback_HandleTypeDef (ADC feedback)

```c
typedef struct
{
    ADC_HandleTypeDef *hadc;      /* Pointer to the ADC handle (e.g., &hadc1)  */
    uint32_t          pollTimeout;/* HAL_ADC_PollForConversion timeout [ms]    */
    uint16_t          adcMin;     /* ADC count corresponding to minAngle       */
    uint16_t          adcMax;     /* ADC count corresponding to maxAngle       */
    float             minAngle;   /* Physical angle at adcMin [deg]            */
    float             maxAngle;   /* Physical angle at adcMax [deg]            */
    uint16_t          lastRaw;    /* Most recent raw ADC reading (output)      */
    float             lastAngle;  /* Most recent computed angle [deg] (output) */
} Servo_Feedback_HandleTypeDef;
```

### 3.3 ADC Feedback Calibration Defaults

| Constant                | Value    | Description                          |
| ----------------------- | -------- | ------------------------------------ |
| `SERVO_FB_ADC_MIN`      | 2435     | ADC count at 0° (from linear fit)    |
| `SERVO_FB_ADC_MAX`      | 57996    | ADC count at 180° (from linear fit)  |
| `SERVO_FB_ANGLE_MIN`    | 0.0f     | Angle at ADC minimum [deg]           |
| `SERVO_FB_ANGLE_MAX`    | 180.0f   | Angle at ADC maximum [deg]           |
| `SERVO_FB_POLL_TIMEOUT` | 10       | Default poll timeout [ms]            |

### 3.4 Public API

#### PWM Output API

| Function                | Description                                     |
| ----------------------- | ----------------------------------------------- |
| `Servo_PWM_Start`       | Starts PWM output on the configured channel     |
| `Servo_SetAngle`        | Sets servo position by angle (0–180°)           |
| `Servo_SetVal`          | Sets servo position by percentage (0–100%)      |
| `Servo_StepTowardAngle` | Increments servo one step toward a target angle |

#### ADC Feedback API

| Function                | Description                                                              |
| ----------------------- | ------------------------------------------------------------------------ |
| `Servo_Feedback_Init`   | Calibrates the ADC (offset, single-ended) and starts it for polling      |
| `Servo_Feedback_Read`   | Triggers one ADC conversion, waits, and computes the feedback angle      |
| `Servo_ComputeAngle`    | Pure computation: converts a raw 16-bit ADC value to an angle [deg]      |

---

## 4. Requirements Specification

### 4.1 System-Level Requirement

| ID          | Description                                 |
| ----------- | ------------------------------------------- |
| SRS-PWM-000 | PWM Servo Module Specification (parent SRS) |

### 4.2 Low-Level Requirements (LLR)

#### Constants & Data Structures

| ID          | Description                                                                                   |
| ----------- | --------------------------------------------------------------------------------------------- |
| LLR-PWM-001 | The servo angle range shall be defined as [0, 180] degrees.                                   |
| LLR-PWM-002 | The servo value range shall be defined as [0, 100] percent.                                   |
| LLR-PWM-003 | Default pulse width limits shall be 530 µs (min) and 2400 µs (max).                           |
| LLR-PWM-004 | A servo handle structure shall encapsulate the timer handle, channel, and pulse width limits. |

#### Servo_PWM_Start

| ID          | Description                                                                            |
| ----------- | -------------------------------------------------------------------------------------- |
| LLR-PWM-010 | The software shall start PWM output on the configured timer channel.                   |
| LLR-PWM-011 | The software shall return an error status if the servo handle or timer handle is NULL. |

#### Servo_SetAngle

| ID          | Description                                                                                 |
| ----------- | ------------------------------------------------------------------------------------------- |
| LLR-PWM-020 | The software shall accept an angle command in the range [0, 180] degrees.                   |
| LLR-PWM-021 | The software shall validate non-NULL servo and timer handles before processing.             |
| LLR-PWM-022 | The software shall clamp the input angle to [SERVO_ANGLE_MIN, SERVO_ANGLE_MAX].             |
| LLR-PWM-023 | The software shall map the clamped angle linearly to a pulse width in [minPulse, maxPulse]. |
| LLR-PWM-024 | The software shall saturate the computed pulse width to the timer period.                   |

#### Servo_SetVal

| ID          | Description                                                                                 |
| ----------- | ------------------------------------------------------------------------------------------- |
| LLR-PWM-030 | The software shall accept a percentage command in the range [0, 100].                       |
| LLR-PWM-031 | The software shall validate non-NULL servo and timer handles before processing.             |
| LLR-PWM-032 | The software shall clamp the input value to [SERVO_VAL_MIN, SERVO_VAL_MAX].                 |
| LLR-PWM-033 | The software shall map the clamped value linearly to a pulse width in [minPulse, maxPulse]. |
| LLR-PWM-034 | The software shall saturate the computed pulse width to the timer period.                   |

#### Servo_StepTowardAngle

| ID          | Description                                                                                 |
| ----------- | ------------------------------------------------------------------------------------------- |
| LLR-PWM-040 | The software shall increment or decrement the current angle toward the target by stepSize.  |
| LLR-PWM-041 | The software shall not overshoot the target angle.                                          |
| LLR-PWM-042 | The software shall return currentAngle unchanged if servo or htim is NULL, or stepSize ≤ 0. |
| LLR-PWM-043 | The software shall apply the intermediate angle via Servo_SetAngle when not yet at target.  |

#### ADC Feedback Constants & Data Structures

| ID          | Description                                                                                          |
| ----------- | ---------------------------------------------------------------------------------------------------- |
| LLR-PWM-050 | ADC feedback calibration defaults shall be defined for a 16-bit ADC (adcMin, adcMax, angle range).   |
| LLR-PWM-051 | A feedback handle structure shall encapsulate the ADC handle, calibration endpoints, and last result. |

#### Servo_Feedback_Init

| ID          | Description                                                                                         |
| ----------- | --------------------------------------------------------------------------------------------------- |
| LLR-PWM-060 | The software shall perform single-ended offset calibration on the ADC before use.                   |
| LLR-PWM-061 | The software shall start the ADC after successful calibration so it is ready for polling.            |
| LLR-PWM-062 | The software shall return HAL_ERROR if the feedback handle or its ADC handle is NULL.                |
| LLR-PWM-063 | The software shall propagate calibration failure without starting the ADC.                           |

#### Servo_Feedback_Read

| ID          | Description                                                                                         |
| ----------- | --------------------------------------------------------------------------------------------------- |
| LLR-PWM-070 | The software shall trigger a new ADC conversion via HAL_ADC_Start.                                  |
| LLR-PWM-071 | The software shall wait for conversion completion using HAL_ADC_PollForConversion with pollTimeout.  |
| LLR-PWM-072 | On successful conversion the software shall store the raw 16-bit value in fb->lastRaw.               |
| LLR-PWM-073 | On successful conversion the software shall compute and store the angle in fb->lastAngle.            |
| LLR-PWM-074 | The software shall return HAL_ERROR if the feedback handle or ADC handle is NULL.                    |
| LLR-PWM-075 | On ADC start or poll failure the software shall leave previous values untouched.                     |

#### Servo_ComputeAngle

| ID          | Description                                                                                         |
| ----------- | --------------------------------------------------------------------------------------------------- |
| LLR-PWM-080 | The software shall return minAngle when raw_adc ≤ adcMin.                                           |
| LLR-PWM-081 | The software shall return maxAngle when raw_adc ≥ adcMax.                                           |
| LLR-PWM-082 | The software shall linearly interpolate the angle for raw_adc values within (adcMin, adcMax).        |
| LLR-PWM-083 | The software shall return 0.0 if the feedback handle pointer is NULL.                               |

---

## 5. Traceability Matrix

### 5.1 Requirements → Source Code

| Requirement  | Source File | Function / Element                | Line Tag                |
| ------------ | ----------- | --------------------------------- | ----------------------- |
| LLR-PWM-001  | pwm_servo.h | `SERVO_ANGLE_*` constants         | `@defgroup SERVO_ANGLE` |
| LLR-PWM-002  | pwm_servo.h | `SERVO_VAL_*` constants           | `@defgroup SERVO_VAL`   |
| LLR-PWM-003  | pwm_servo.h | `SERVO_*_PULSE` constants         | `@defgroup SERVO_PULSE` |
| LLR-PWM-004  | pwm_servo.h | `Servo_HandleTypeDef`             | Struct declaration      |
| LLR-PWM-010  | pwm_servo.c | `Servo_PWM_Start`                 | `@req LLR-PWM-010`      |
| LLR-PWM-011  | pwm_servo.c | `Servo_PWM_Start`                 | `@req LLR-PWM-011`      |
| LLR-PWM-020  | pwm_servo.c | `Servo_SetAngle`                  | `@req LLR-PWM-020`      |
| LLR-PWM-021  | pwm_servo.c | `Servo_SetAngle`                  | `@req LLR-PWM-021`      |
| LLR-PWM-022  | pwm_servo.c | `Servo_SetAngle`                  | `@req LLR-PWM-022`      |
| LLR-PWM-023  | pwm_servo.c | `Servo_SetAngle`                  | `@req LLR-PWM-023`      |
| LLR-PWM-024  | pwm_servo.c | `Servo_SetAngle`                  | `@req LLR-PWM-024`      |
| LLR-PWM-030  | pwm_servo.c | `Servo_SetVal`                    | `@req LLR-PWM-030`      |
| LLR-PWM-031  | pwm_servo.c | `Servo_SetVal`                    | `@req LLR-PWM-031`      |
| LLR-PWM-032  | pwm_servo.c | `Servo_SetVal`                    | `@req LLR-PWM-032`      |
| LLR-PWM-033  | pwm_servo.c | `Servo_SetVal`                    | `@req LLR-PWM-033`      |
| LLR-PWM-034  | pwm_servo.c | `Servo_SetVal`                    | `@req LLR-PWM-034`      |
| LLR-PWM-040  | pwm_servo.c | `Servo_StepTowardAngle`           | `@req LLR-PWM-040`      |
| LLR-PWM-041  | pwm_servo.c | `Servo_StepTowardAngle`           | `@req LLR-PWM-041`      |
| LLR-PWM-042  | pwm_servo.c | `Servo_StepTowardAngle`           | `@req LLR-PWM-042`      |
| LLR-PWM-043  | pwm_servo.c | `Servo_StepTowardAngle`           | `@req LLR-PWM-043`      |
| LLR-PWM-050  | pwm_servo.h | `SERVO_FB_*` constants            | `@defgroup SERVO_FB`    |
| LLR-PWM-051  | pwm_servo.h | `Servo_Feedback_HandleTypeDef`    | Struct declaration      |
| LLR-PWM-060  | pwm_servo.c | `Servo_Feedback_Init`             | `@req LLR-PWM-060`      |
| LLR-PWM-061  | pwm_servo.c | `Servo_Feedback_Init`             | `@req LLR-PWM-061`      |
| LLR-PWM-062  | pwm_servo.c | `Servo_Feedback_Init`             | `@req LLR-PWM-062`      |
| LLR-PWM-063  | pwm_servo.c | `Servo_Feedback_Init`             | `@req LLR-PWM-063`      |
| LLR-PWM-070  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-070`      |
| LLR-PWM-071  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-071`      |
| LLR-PWM-072  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-072`      |
| LLR-PWM-073  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-073`      |
| LLR-PWM-074  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-074`      |
| LLR-PWM-075  | pwm_servo.c | `Servo_Feedback_Read`             | `@req LLR-PWM-075`      |
| LLR-PWM-080  | pwm_servo.c | `Servo_ComputeAngle`              | `@req LLR-PWM-080`      |
| LLR-PWM-081  | pwm_servo.c | `Servo_ComputeAngle`              | `@req LLR-PWM-081`      |
| LLR-PWM-082  | pwm_servo.c | `Servo_ComputeAngle`              | `@req LLR-PWM-082`      |
| LLR-PWM-083  | pwm_servo.c | `Servo_ComputeAngle`              | `@req LLR-PWM-083`      |

### 5.2 Requirements → Test Cases

| Requirement  | Test Case(s)           | Description                                                    |
| ------------ | ---------------------- | -------------------------------------------------------------- |
| LLR-PWM-010  | TC-PWM-010             | Verify PWM starts successfully with valid handle               |
| LLR-PWM-011  | TC-PWM-011, TC-PWM-012 | Verify error on NULL servo / NULL htim                         |
| LLR-PWM-020  | TC-PWM-020             | Verify angle 90° produces midpoint pulse width                 |
| LLR-PWM-021  | TC-PWM-021             | Verify HAL_ERROR when servo is NULL                            |
| LLR-PWM-022  | TC-PWM-022, TC-PWM-023 | Verify clamping at angle < 0 and angle > 180                   |
| LLR-PWM-023  | TC-PWM-020             | Verified via midpoint calculation                              |
| LLR-PWM-024  | TC-PWM-024             | Verify pulse saturation at timer period                        |
| LLR-PWM-030  | TC-PWM-030             | Verify val 50 produces midpoint pulse width                    |
| LLR-PWM-031  | TC-PWM-031             | Verify HAL_ERROR when servo is NULL                            |
| LLR-PWM-032  | TC-PWM-032, TC-PWM-033 | Verify clamping at val < 0 and val > 100                       |
| LLR-PWM-033  | TC-PWM-030             | Verified via midpoint calculation                              |
| LLR-PWM-034  | TC-PWM-034             | Verify pulse saturation at timer period                        |
| LLR-PWM-040  | TC-PWM-040, TC-PWM-041 | Verify step up (0→90) and step down (180→90)                   |
| LLR-PWM-041  | TC-PWM-042             | Verify no overshoot when remaining < stepSize                  |
| LLR-PWM-042  | TC-PWM-043, TC-PWM-044 | Verify unchanged return on NULL / stepSize ≤ 0                 |
| LLR-PWM-043  | TC-PWM-040, TC-PWM-041 | Verify Servo_SetAngle called at intermediate                   |
| LLR-PWM-050  | TC-PWM-050             | Verify calibration constants match header defaults             |
| LLR-PWM-051  | TC-PWM-051             | Verify feedback handle struct layout and field types           |
| LLR-PWM-060  | TC-PWM-060             | Verify ADC offset calibration is called on init                |
| LLR-PWM-061  | TC-PWM-061             | Verify ADC is started after successful calibration             |
| LLR-PWM-062  | TC-PWM-062, TC-PWM-063 | Verify HAL_ERROR on NULL fb / NULL hadc                        |
| LLR-PWM-063  | TC-PWM-064             | Verify ADC not started when calibration fails                  |
| LLR-PWM-070  | TC-PWM-070             | Verify a conversion is triggered via HAL_ADC_Start             |
| LLR-PWM-071  | TC-PWM-071             | Verify poll waits up to pollTimeout ms                         |
| LLR-PWM-072  | TC-PWM-072             | Verify lastRaw stores the 16-bit ADC result                    |
| LLR-PWM-073  | TC-PWM-073             | Verify lastAngle is computed via Servo_ComputeAngle            |
| LLR-PWM-074  | TC-PWM-074, TC-PWM-075 | Verify HAL_ERROR on NULL fb / NULL hadc                        |
| LLR-PWM-075  | TC-PWM-076             | Verify previous values preserved on conversion failure         |
| LLR-PWM-080  | TC-PWM-080             | Verify minAngle returned when raw_adc ≤ adcMin                 |
| LLR-PWM-081  | TC-PWM-081             | Verify maxAngle returned when raw_adc ≥ adcMax                 |
| LLR-PWM-082  | TC-PWM-082             | Verify linear interpolation for mid-range raw_adc values       |
| LLR-PWM-083  | TC-PWM-083             | Verify 0.0 returned when fb is NULL                            |

---

## 6. Configuration Management

| Item             | Value                        |
| ---------------- | ---------------------------- |
| CI Identifier    | CI-CM7-PWM                   |
| CSCI Identifier  | CSCI-SERVO-CTRL              |
| Baseline Version | 1.0.0                        |
| Source Files     | `pwm_servo.h`, `pwm_servo.c`  |
| VCS              | Git                          |

All source files shall be under version control. Each release baseline shall be
tagged and traceable to the corresponding SDD version.

---

## 7. Derived Requirements

Any requirement in this module that does **not** trace upward to a parent HLR
or SRS entry must be flagged as a **Derived Requirement** using the `@derivedrq`
tag. Derived requirements require additional safety assessment review per
DO-178B §5.1.2.

Currently, **no derived requirements** have been identified.

---

## 8. Revision History

| Version | Date       | Author | Description                                                                  |
| ------- | ---------- | ------ | ---------------------------------------------------------------------------- |
| 1.0.0   | 2025-04-02 |        | Initial draft — PWM output API (Start, SetAngle, SetVal, StepTowardAngle)   |
| 2.0.0   | 2025-04-09 |        | Added ADC feedback subsystem (Feedback_Init, Feedback_Read, ComputeAngle)   |

---
