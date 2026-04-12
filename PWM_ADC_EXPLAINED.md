# PWM Output & ADC Feedback — Technical Explanation

This document explains how the PWM servo output and ADC position feedback
subsystems work in the `pwm_servo` module, how raw hardware values are
converted to physical quantities, and how the calibration constants were
derived.

---

## 1. System Overview

The servo control loop has two data paths:

```
                    ┌──────────────────────────┐
  Commanded angle   │    pwm_servo module       │
  ─────────────────►│                           │
                    │  Servo_SetAngle()          │    PWM signal
                    │  angle → pulse width ──────┼──────────────► Servo motor
                    │                           │                  (PA0 / TIM2_CH1)
                    │  Servo_ComputeAngle()      │    Analog voltage
  Measured angle  ◄─┤  ADC count → angle ◄──────┼──────────────── Servo potentiometer
                    │  Servo_Feedback_Read()     │                  (PF11 / ADC1 INP2)
                    └──────────────────────────┘
```

| Path            | Direction           | Peripheral | Pin  | Signal           |
| --------------- | ------------------- | ---------- | ---- | ---------------- |
| **PWM output**  | MCU → Servo         | TIM2 CH1   | PA0  | 50 Hz PWM        |
| **ADC feedback**| Servo → MCU         | ADC1 INP2  | PF11 | 0 – 3.3 V analog |

---

## 2. PWM Output Path

### 2.1 What Is PWM?

Pulse Width Modulation (PWM) is a digital technique where a periodic signal's
**duty cycle** (the fraction of the period during which the signal is HIGH)
encodes an analog value. Hobby servos decode the **pulse width** (HIGH time)
of a 50 Hz signal to determine shaft position:

```
        ◄──────── Period = 20 ms (50 Hz) ────────►
        ┌──────┐                                   ┌──────┐
        │      │                                   │      │
   ─────┘      └───────────────────────────────────┘      └─────
        ◄─────►
       Pulse width
      (530 – 2400 µs)
```

| Pulse Width | Servo Angle |
| ----------- | ----------- |
| 530 µs      | 0°          |
| ~1465 µs    | 90°         |
| 2400 µs     | 180°        |

### 2.2 Timer Configuration (TIM2)

The STM32H755 CM7 core drives the PWM signal through **TIM2 Channel 1**,
routed to pin **PA0** via alternate function AF1.

| Parameter             | Value   | Derivation                                              |
| --------------------- | ------- | ------------------------------------------------------- |
| Timer clock (APB1)    | 200 MHz | SYSCLK 400 MHz ÷ AHB /2 = 200 MHz, APB1 ×2 = 200 MHz  |
| Prescaler (PSC)       | 199     | 200 MHz ÷ (199 + 1) = **1 MHz** → 1 count = **1 µs**   |
| Auto-Reload (ARR)     | 19 999  | (19 999 + 1) counts × 1 µs = **20 ms** → **50 Hz**     |
| Counter mode          | Up      |                                                         |
| Output compare mode   | PWM1    | Output HIGH while counter < CCR                        |

Because 1 timer count = 1 µs, the pulse-width constants are specified
directly in microseconds:

```c
#define SERVO_MIN_PULSE  530U    /* 530 µs → 0°   */
#define SERVO_MAX_PULSE  2400U   /* 2400 µs → 180° */
```

### 2.3 Angle-to-Pulse Conversion

`Servo_SetAngle()` converts a commanded angle (0–180°) to a timer compare
value (pulse width in counts = µs) using **linear interpolation**:

```
pulse = minPulse + (maxPulse − minPulse) × angle / 180
```

In C:

```c
range = servo->maxPulse - servo->minPulse;            /* 2400 − 530 = 1870 */
pulse = servo->minPulse + (range * (uint32_t)angle) / SERVO_ANGLE_RANGE;
```

#### Worked example — 90°

```
pulse = 530 + (1870 × 90) / 180
      = 530 + 168300 / 180
      = 530 + 935
      = 1465 µs
```

The computed pulse is **clamped** to the timer period (19 999 µs) as a
safety saturation before being written to the capture/compare register:

```c
if (pulse > servo->htim->Init.Period) {
    pulse = servo->htim->Init.Period;
}
__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse);
```

### 2.4 Percentage-to-Pulse Conversion

`Servo_SetVal()` operates identically but maps from [0, 100]% instead of
[0, 180]°:

```
pulse = minPulse + (maxPulse − minPulse) × val / 100
```

---

## 3. ADC Feedback Path

### 3.1 Hardware Setup

The servo used is an **FS90-FB** (or equivalent feedback servo) which has a
**4th wire** connected to its internal potentiometer. As the shaft rotates
from 0° to 180°, the wiper voltage varies approximately linearly between two
extremes within the 0–3.3 V range.

| Item             | Detail                   |
| ---------------- | ------------------------ |
| ADC peripheral   | ADC1                     |
| Input channel    | INP2 (input positive 2)  |
| GPIO pin         | PF11                     |
| ADC resolution   | 16-bit (0 – 65 535)      |
| Reference voltage| 3.3 V (VREF+)            |
| Conversion mode  | Single polling           |

### 3.2 ADC Initialization

Before reading, the ADC undergoes:

1. **Offset calibration** — `HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)`
   removes internal offset errors in the ADC's comparator chain.
2. **Start** — `HAL_ADC_Start(hadc)` arms the ADC for subsequent
   software-triggered conversions.

Both steps are performed in `Servo_Feedback_Init()`. If offset calibration
fails, the ADC is **not** started (fail-safe behavior).

### 3.3 ADC Reading Flow

Each call to `Servo_Feedback_Read()`:

```
HAL_ADC_Start()               ← Trigger a new conversion
    │
    ▼
HAL_ADC_PollForConversion()   ← Wait up to pollTimeout ms
    │
    ▼
HAL_ADC_GetValue()            ← Read the 16-bit result (0 – 65 535)
    │
    ▼
Servo_ComputeAngle()          ← Convert raw count → angle [deg]
```

On success, both `fb->lastRaw` and `fb->lastAngle` are updated. On failure,
previous values are **preserved** (no corruption of stale data).

---

## 4. ADC Calibration — Deriving `adcMin` and `adcMax`

### 4.1 Why Calibration Is Needed

The ADC produces a **raw integer count** (0–65 535 for 16-bit resolution).
This count must be mapped to a physical angle (0°–180°). The mapping depends
on:

- The potentiometer's voltage divider ratio at each mechanical endpoint.
- The ADC reference voltage (3.3 V).
- Component tolerances (varies unit-to-unit).

There is no universal formula; the mapping constants must be determined
**empirically** for each servo unit (or servo model, with tolerances
accepted).

### 4.2 Calibration Procedure

The calibration follows a simple **two-point linear fit**:

#### Step 1 — Command the servo to 0° and record the ADC reading

```
Servo_SetAngle(&servo, 0);
osDelay(500);                             // Allow servo to settle
Servo_Feedback_Read(&servoFb);
// Record servoFb.lastRaw → e.g. 2435
```

#### Step 2 — Command the servo to 180° and record the ADC reading

```
Servo_SetAngle(&servo, 180);
osDelay(500);
Servo_Feedback_Read(&servoFb);
// Record servoFb.lastRaw → e.g. 57996
```

#### Step 3 — Store the results as calibration constants

```c
#define SERVO_FB_ADC_MIN   2435U    /* ADC count at 0°   */
#define SERVO_FB_ADC_MAX   57996U   /* ADC count at 180° */
```

> **Tip**: For higher accuracy, repeat each measurement several times and
> average the results. You can also sample at intermediate angles (e.g., 45°,
> 90°, 135°) to verify linearity.

### 4.3 Voltage Estimation

Knowing the ADC resolution and VREF, you can estimate the potentiometer
voltages at the endpoints:

```
V = (ADC_count / 65535) × 3.3 V
```

| Angle | ADC Count | Estimated Voltage |
| ----- | --------- | ----------------- |
| 0°    | 2 435     | 0.122 V           |
| 180°  | 57 996    | 2.919 V           |

This shows the potentiometer sweeps from ≈ 0.12 V to ≈ 2.92 V over the
0°–180° range (≈ 2.80 V span).

### 4.4 The Linear Interpolation Formula

`Servo_ComputeAngle()` converts a raw ADC reading to an angle using the
standard linear interpolation (two-point form):

```
              (maxAngle − minAngle)
angle = minAngle + ─────────────────────── × (raw_adc − adcMin)
              (adcMax − adcMin)
```

Which simplifies to:

```
angle = 0.0 + (180.0 / (57996 − 2435)) × (raw_adc − 2435)
      = 180.0 × (raw_adc − 2435) / 55561
```

#### Worked Example

| raw_adc | Calculation                       | Result    |
| ------- | --------------------------------- | --------- |
| 2 435   | 180 × (2435 − 2435) / 55561      | **0.0°**  |
| 30 216  | 180 × (30216 − 2435) / 55561     | **90.0°** |
| 57 996  | 180 × (57996 − 2435) / 55561     | **180.0°**|
| 1 000   | clamped → raw_adc ≤ adcMin        | **0.0°**  |
| 60 000  | clamped → raw_adc ≥ adcMax        | **180.0°**|

### 4.5 Clamping / Saturation Behavior

The function enforces bounds (protective clamping):

| Condition            | Output            |
| -------------------- | ----------------- |
| `raw_adc ≤ adcMin`   | Returns `minAngle` (0.0°)   |
| `raw_adc ≥ adcMax`   | Returns `maxAngle` (180.0°) |
| `adcMin < raw_adc < adcMax` | Linear interpolation |
| `fb == NULL`         | Returns 0.0 (fail-safe) |

This ensures out-of-range ADC readings (noise, wiring faults) never produce
nonsensical angle values.

---

## 5. End-to-End Signal Flow

The complete closed-loop data path in the 10 ms servo task:

```
┌─────────────────────────────────────────────────────────────────┐
│  servoTask (10 ms period)                                       │
│                                                                 │
│  1. Read button → update Simulink model input (rtU.Button)      │
│  2. Step toward model output angle:                             │
│        currentAngle = Servo_StepTowardAngle(servo, feedback,    │
│                                             rtY.angle, 2)       │
│        └─► Servo_SetAngle() → TIM2 CCR1 → PA0 PWM               │
│                                                                 │
│  3. Read ADC feedback:                                          │
│        Servo_Feedback_Read(&servoFb)                            │
│        └─► HAL_ADC_Start → Poll → GetValue → ComputeAngle       │
│        └─► servoFb.lastAngle (measured angle)                   │
│                                                                 │
│  4. Feed measured angle back to Simulink model:                 │
│        rtU.FeedbackServo = servoFb.lastAngle                    │
│                                                                 │
│  5. Step the Simulink model:                                    │
│        flap_step()   → produces rtY.angle for next iteration    │
│                                                                 │
│  6. osDelay(10)  ← 10 ms period                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 6. Summary of Calibration Constants

| Constant             | Value    | Unit    | Meaning                                |
| -------------------- | -------- | ------- | -------------------------------------- |
| `SERVO_MIN_PULSE`    | 530      | µs      | PWM pulse width at 0°                  |
| `SERVO_MAX_PULSE`    | 2400     | µs      | PWM pulse width at 180°                |
| `SERVO_FB_ADC_MIN`   | 2 435    | counts  | 16-bit ADC reading at 0° (≈ 0.12 V)    |
| `SERVO_FB_ADC_MAX`   | 57 996   | counts  | 16-bit ADC reading at 180° (≈ 2.92 V)  |
| `SERVO_FB_ANGLE_MIN` | 0.0      | degrees | Angle at minimum ADC count             |
| `SERVO_FB_ANGLE_MAX` | 180.0    | degrees | Angle at maximum ADC count             |
| `SERVO_FB_POLL_TIMEOUT` | 10    | ms      | ADC poll timeout per conversion        |

> **Note**: `SERVO_MIN_PULSE` / `SERVO_MAX_PULSE` are characteristic of the
> servo model (datasheet values). `SERVO_FB_ADC_MIN` / `SERVO_FB_ADC_MAX` are
> **empirically measured** and may differ between individual servo units.

---

## 7. Re-Calibration Checklist

If you replace the servo or need to re-calibrate:

1. Command 0° → wait for settling → record raw ADC count → new `SERVO_FB_ADC_MIN`.
2. Command 180° → wait for settling → record raw ADC count → new `SERVO_FB_ADC_MAX`.
3. (Optional) Verify linearity at 45°, 90°, 135°.
4. Update `#define` values in `pwm_servo.h`.
5. Rebuild and re-test.

---
