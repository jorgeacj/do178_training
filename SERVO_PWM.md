# PWM Servo Control Module — Software Design Document

| Field                  | Value           |
| ---------------------- | --------------- |
| **Document ID**        | SDD-CM7-PWM-001 |
| **Version**            | 1.0.0           |
| **Date**               | 2025-04-02      |
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

```c
typedef struct
{
    TIM_HandleTypeDef *htim;   /* Pointer to the timer handle  */
    uint32_t channel;          /* Timer channel                */
    uint32_t minPulse;         /* Minimum pulse width (counts) */
    uint32_t maxPulse;         /* Maximum pulse width (counts) */
} Servo_HandleTypeDef;
```

### 3.3 Public API

| Function                | Description                                     |
| ----------------------- | ----------------------------------------------- |
| `Servo_PWM_Start`       | Starts PWM output on the configured channel     |
| `Servo_SetAngle`        | Sets servo position by angle (0–180°)           |
| `Servo_SetVal`          | Sets servo position by percentage (0–100%)      |
| `Servo_StepTowardAngle` | Increments servo one step toward a target angle |

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

---

## 5. Traceability Matrix

### 5.1 Requirements → Source Code

| Requirement | Source File | Function / Element        | Line Tag                |
| ----------- | ----------- | ------------------------- | ----------------------- |
| LLR-PWM-001 | pwm_servo.h | `SERVO_ANGLE_*` constants | `@defgroup SERVO_ANGLE` |
| LLR-PWM-002 | pwm_servo.h | `SERVO_VAL_*` constants   | `@defgroup SERVO_VAL`   |
| LLR-PWM-003 | pwm_servo.h | `SERVO_*_PULSE` constants | `@defgroup SERVO_PULSE` |
| LLR-PWM-004 | pwm_servo.h | `Servo_HandleTypeDef`     | Struct declaration      |
| LLR-PWM-010 | pwm_servo.c | `Servo_PWM_Start`         | `@req LLR-PWM-010`      |
| LLR-PWM-011 | pwm_servo.c | `Servo_PWM_Start`         | `@req LLR-PWM-011`      |
| LLR-PWM-020 | pwm_servo.c | `Servo_SetAngle`          | `@req LLR-PWM-020`      |
| LLR-PWM-021 | pwm_servo.c | `Servo_SetAngle`          | `@req LLR-PWM-021`      |
| LLR-PWM-022 | pwm_servo.c | `Servo_SetAngle`          | `@req LLR-PWM-022`      |
| LLR-PWM-023 | pwm_servo.c | `Servo_SetAngle`          | `@req LLR-PWM-023`      |
| LLR-PWM-024 | pwm_servo.c | `Servo_SetAngle`          | `@req LLR-PWM-024`      |
| LLR-PWM-030 | pwm_servo.c | `Servo_SetVal`            | `@req LLR-PWM-030`      |
| LLR-PWM-031 | pwm_servo.c | `Servo_SetVal`            | `@req LLR-PWM-031`      |
| LLR-PWM-032 | pwm_servo.c | `Servo_SetVal`            | `@req LLR-PWM-032`      |
| LLR-PWM-033 | pwm_servo.c | `Servo_SetVal`            | `@req LLR-PWM-033`      |
| LLR-PWM-034 | pwm_servo.c | `Servo_SetVal`            | `@req LLR-PWM-034`      |
| LLR-PWM-040 | pwm_servo.c | `Servo_StepTowardAngle`   | `@req LLR-PWM-040`      |
| LLR-PWM-041 | pwm_servo.c | `Servo_StepTowardAngle`   | `@req LLR-PWM-041`      |
| LLR-PWM-042 | pwm_servo.c | `Servo_StepTowardAngle`   | `@req LLR-PWM-042`      |
| LLR-PWM-043 | pwm_servo.c | `Servo_StepTowardAngle`   | `@req LLR-PWM-043`      |

### 5.2 Requirements → Test Cases

| Requirement | Test Case(s)           | Description                                      |
| ----------- | ---------------------- | ------------------------------------------------ |
| LLR-PWM-010 | TC-PWM-010             | Verify PWM starts successfully with valid handle |
| LLR-PWM-011 | TC-PWM-011, TC-PWM-012 | Verify error on NULL servo / NULL htim           |
| LLR-PWM-020 | TC-PWM-020             | Verify angle 90° produces midpoint pulse width   |
| LLR-PWM-021 | TC-PWM-021             | Verify HAL_ERROR when servo is NULL              |
| LLR-PWM-022 | TC-PWM-022, TC-PWM-023 | Verify clamping at angle < 0 and angle > 180     |
| LLR-PWM-023 | TC-PWM-020             | Verified via midpoint calculation                |
| LLR-PWM-024 | TC-PWM-024             | Verify pulse saturation at timer period          |
| LLR-PWM-030 | TC-PWM-030             | Verify val 50 produces midpoint pulse width      |
| LLR-PWM-031 | TC-PWM-031             | Verify HAL_ERROR when servo is NULL              |
| LLR-PWM-032 | TC-PWM-032, TC-PWM-033 | Verify clamping at val < 0 and val > 100         |
| LLR-PWM-033 | TC-PWM-030             | Verified via midpoint calculation                |
| LLR-PWM-034 | TC-PWM-034             | Verify pulse saturation at timer period          |
| LLR-PWM-040 | TC-PWM-040, TC-PWM-041 | Verify step up (0→90) and step down (180→90)     |
| LLR-PWM-041 | TC-PWM-042             | Verify no overshoot when remaining < stepSize    |
| LLR-PWM-042 | TC-PWM-043, TC-PWM-044 | Verify unchanged return on NULL / stepSize ≤ 0   |
| LLR-PWM-043 | TC-PWM-040, TC-PWM-041 | Verify Servo_SetAngle called at intermediate     |

---

## 6. Configuration Management

| Item             | Value                        |
| ---------------- | ---------------------------- |
| CI Identifier    | CI-CM7-PWM                   |
| CSCI Identifier  | CSCI-SERVO-CTRL              |
| Baseline Version | 1.0.0                        |
| Source Files     | `pwm_servo.h`, `pwm_servo.c` |
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

| Version | Date       | Author | Description               |
| ------- | ---------- | ------ | ------------------------- |
| 1.0.0   | 2025-04-02 |        | Initial draft             |


---
