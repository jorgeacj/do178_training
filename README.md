# do178_training
This repository contains a project for training embedded software development in comply with DO178C.

## 1. Roadmap

### Program setup / logistics
- [X] Purchase STM32 devkit, stepper motor, stepper driver (STEP/DIR/EN), encoder, limit switches, PSU, cables/adapters
- [X] Install STM32CubeIDE + STM32CubeMX (+ drivers) and record tool versions (toolchain baseline)
- [X] Create repository + folder structure (`src/inc/app/bsp/test/doc/tools`) + build instructions
- [ ] Define evidence storage convention (build logs, configs, reports per tag)
 
### DO-178C / DO-330 foundations (study artifacts)
- [ ] Write “Tailored DO-178C approach for this exercise” (lifecycle + objectives mapping)
- [ ] Write DO-330 basics note (when tool qualification is needed, TQL concept)
- [ ] Create tool inventory (name/version/purpose/qualification stance)
 
### Requirements / architecture / traceability
- [ ] Create requirements set with IDs (motor, encoder, homing, safety, comms, timing, constraints)
- [ ] Select capstone requirement subset (25 reqs) for full traceability
- [ ] Create architecture document (modules, interfaces, task rates, ISR rules, data ownership)
- [ ] Define traceability method (Req <> design <> code <> tests) + examples
 
### Coding rules / compliance constraints
- [X] Define coding standard (MISRA-oriented subset + project rules) + deviation process template
- [X] Enforce “no dynamic allocation after init” rule (malloc/free banned) + build check
- [X] Enforce “no HAL calls from application” (BSP wrapper-only access)
 
### BSP / platform framework (HAL/LL strategy)
- [ ] Implement BSP wrappers: GPIO, timebase, timer, UART, CAN, watchdog, EXTI
- [ ] Define and document ISR policy (minimal ISR, defer work, shared data rules)
- [X] Set up scheduler/superloop structure with fixed-rate tasks
 
### Timing / determinism
- [ ] Implement monotonic time base + overflow behavior
- [X] Implement periodic execution model (e.g., 1 ms tick + 10 ms control loop)
- [ ] Measure and document control-loop jitter / step pulse timing jitter (bounded jitter requirement)
 
### Stepper motor control (driver layer)
- [X] Implement STEP pulse generation using timer-based mechanism
- [X] Implement DIR/EN control and safe disable behavior
- [X] Implement speed limiting and acceleration limiting support in driver/control boundary
 
### Encoder integration (feedback)
- [ ] Configure timer encoder mode for quadrature A/B
- [ ] Implement signed position accumulator + rollover handling
- [ ] Implement counts-to-engineering-units scaling
- [ ] Implement encoder plausibility checks (max delta per cycle, direction consistency)
- [ ] Implement encoder loss detection during commanded motion (timeout-based)
 
### Limit switches + homing
- [ ] Integrate HOME and MAX limit switches (EXTI + debounce)
- [ ] Implement homing state machine (approach, detect, back-off, set zero/offset)
- [ ] Implement homing timeout fault behavior
- [ ] Implement MAX hit during motion -> fault + stop + outputs disabled
 
### Motion control / behavior
- [ ] Implement motion profile (trapezoid or S-curve) and closed-loop move-to-position
- [ ] Implement “target reached” logic (tolerance + settle time)
- [ ] Implement STOP command behavior (safe halt state)
- [ ] Implement command rejection for out-of-range requests (min/max limits)
 
### Safety / fault management / watchdog
- [ ] Implement fault manager (NO_FAULT / ACTIVE / LATCHED) + fault codes
- [ ] Implement comm timeout -> fault + motion stop
- [ ] Integrate watchdog (nominal service + expiry demonstration case)
- [ ] Define fault reset / recovery rules (when allowed, whether homing required)
 
### Communications — RS232 (primary)
- [ ] Define RS232 ICD (frame format, fields, scaling, error handling)
- [ ] Implement RS232 command parser (move, stop, home, reset, config if needed)
- [ ] Implement RS232 telemetry (state, position, faults, timestamps)
- [ ] Implement robust invalid-frame handling (no unsafe behavior)
 
### Communications — CAN (secondary/extended)
- [ ] Define CAN message dictionary (IDs, payload layout, scaling)
- [ ] Implement CAN command reception and validation
- [ ] Implement CAN telemetry transmission
- [ ] (Optional) CAN robustness features (filters, error counters, bus-off handling)
 
### Verification infrastructure
- [ ] Integrate unit test framework (host-based preferred) for algorithmic modules
- [ ] Create integration test procedure template (standard format, evidence captured)
- [ ] Populate and execute integration test matrix (IT-001 ...  IT-025)
- [ ] Archive test evidence per baseline tag (logs, captures, results)
 
### Coupling exercises (design-for-verification)
- [ ] Write Data Coupling guideline + refactor one module to reduce hidden dependencies
- [ ] Write Control Coupling guideline + refactor to replace flag-driven coupling with explicit APIs/events
- [ ] Demonstrate improved testability (unit tests updated accordingly)
 
### MCDC (intro + applied)
- [ ] Write MCDC intro note with worked example
- [ ] Select MCDC target function (recommended: motion/safety state machine step function)
- [ ] Produce truth table + minimal MCDC test set
- [ ] Implement MCDC-oriented unit tests for target function
- [ ] (Tool-dependent) Generate coverage report or document coverage procedure/limitations
 
### Static analysis / reports / evidence packaging (exercise-level)
- [ ] Enable compiler warning gates and/or static analysis checks
- [ ] Archive reports/configs/tool versions per release tag (DO-330 mindset)
- [ ] Perform final “evidence packaging check” against capstone subset and test matrix





## 2. Applicable Documents

| ID        | Title                                                       |
| --------- | ----------------------------------------------------------- |
| DO-178B   | Software Considerations in Airborne Systems and Equipment   |
| DO-178C   | Software Considerations in Airborne Systems (successor)     |
| MISRA-C   | MISRA C:2012 — Guidelines for the Use of the C Language     |
| ARP-4754A | Guidelines for Development of Civil Aircraft and Systems    |
| ARP-4761  | Guidelines and Methods for Conducting the Safety Assessment |

---

## 3. Design Assurance Level (DAL) Classification

### 3.1 DAL Definitions

| Level     | Failure Condition | Description                                                      |
| --------- | ----------------- | ---------------------------------------------------------------- |
| **DAL-A** | Catastrophic      | Prevents continued safe flight and landing                       |
| **DAL-B** | Hazardous         | Large reduction in safety margins or crew capability             |
| **DAL-C** | Major             | Significant reduction in safety margins, increased crew workload |
| **DAL-D** | Minor             | Slight reduction in safety margins                               |
| **DAL-E** | No Effect         | No impact on aircraft safety or operations                       |

### 3.2 DAL Assignment Process

The DAL is assigned through the **System Safety Assessment (SSA)** process per
ARP-4761, not by the software development team. The process follows:

1. **Functional Hazard Assessment (FHA)** — Identifies all failure conditions
   for the system function.
2. **Preliminary System Safety Assessment (PSSA)** — Allocates failure
   conditions to hardware/software subsystems.
3. **System Safety Assessment (SSA)** — Validates the allocation and confirms
   the assigned DAL.

### 3.3 DAL Verification Objectives

| Objective                        | DAL-A | DAL-B | DAL-C | DAL-D | DAL-E |
| -------------------------------- | :---: | :---: | :---: | :---: | :---: |
| Requirements-Based Testing       |  (X)   |  (X)   |  (X)   |  (X)   |   —   |
| Statement Coverage (SC)          |  (X)   |  (X)   |  (X)   |   —   |   —   |
| Decision Coverage (DC)           |  (X)   |  (X)   |  —   |   —   |   —   |
| MC/DC Coverage                   |  (X)   |  —   |   —   |   —   |   —   |
| Traceability (Req <> Code <> Test) |  (X)   |  (X)   |  (X)   |  (X)   |   —   |
| Independence of Verification     |  (X)   |  (X)   |   —   |   —   |   —   |




## Annex A — Acronyms and Abbreviations

| Acronym / Abbreviation | Full Form                                       | Context                                                                 |
| ---------------------- | ----------------------------------------------- | ----------------------------------------------------------------------- |
| **API**                | Application Programming Interface               | The set of public functions exposed by a module                         |
| **ARP**                | Aerospace Recommended Practice                  | SAE standard designation (e.g., ARP-4754A, ARP-4761)                    |
| **ASIL**               | Automotive Safety Integrity Level               | ISO 26262 safety level (A–D); automotive equivalent of DAL              |
| **CI**                 | Configuration Item                              | A unit of work product placed under configuration management            |
| **CM**                 | Configuration Management                        | The discipline of tracking and controlling changes to artifacts         |
| **CSCI**               | Computer Software Configuration Item            | A top-level software element identified for configuration control       |
| **DAL**                | Design Assurance Level                          | DO-178B safety classification (A–E) based on failure severity           |
| **DC**                 | Decision Coverage                               | A structural coverage metric: every decision (true/false) exercised     |
| **DO-178B/C**          | Design Organization standard 178B / 178C        | RTCA/EUROCAE standard for airborne software certification               |
| **FHA**                | Functional Hazard Assessment                    | First step of safety assessment: identifies failure conditions          |
| **HAL**                | Hardware Abstraction Layer                      | STM32 library layer abstracting direct register access                  |
| **HLR**                | High-Level Requirement                          | A requirement derived from system requirements, describing _what_       |
| **HWCI**               | Hardware Configuration Item                     | Hardware counterpart of CSCI                                            |
| **LLR**                | Low-Level Requirement                           | A requirement describing _how_ to implement an HLR; design-level        |
| **MC/DC**              | Modified Condition / Decision Coverage          | Structural coverage metric required for DAL-A and DAL-B                 |
| **MISRA**              | Motor Industry Software Reliability Association | Organization that publishes C/C++ coding guidelines                     |
| **OCD**                | Operational Concepts Document                   | Describes how the system is intended to be operated                     |
| **PHAC**               | Plan for Hardware Aspects of Certification      | Hardware certification planning document                                |
| **PSAC**               | Plan for Software Aspects of Certification      | Master planning document for DO-178B certification                      |
| **PSSA**               | Preliminary System Safety Assessment            | Allocates failure conditions to HW/SW subsystems                        |
| **PWM**                | Pulse Width Modulation                          | Digital signal technique for controlling servos/motors                  |
| **RTCA**               | Radio Technical Commission for Aeronautics      | Organization that publishes DO-178B/C                                   |
| **SC**                 | Statement Coverage                              | Structural coverage metric: every executable statement reached          |
| **SCI**                | Software Configuration Index                    | Index listing all deliverable software items in a baseline              |
| **SDD**                | Software Design Document                        | Describes the software architecture and low-level design                |
| **SDP**                | Software Development Plan                       | Defines the processes, standards, and tools for development             |
| **SRS**                | Software Requirements Specification             | Document containing all high-level software requirements                |
| **SSA**                | System Safety Assessment                        | Final safety analysis validating DAL assignments                        |
| **SVP**                | Software Verification Plan                      | Defines the verification strategy, methods, and tools                   |
| **TC**                 | Test Case                                       | A single verification step with inputs, procedure, and expected results |
| **TIM**                | Timer (STM32 peripheral)                        | Hardware timer used to generate PWM signals                             |
| **VCS**                | Version Control System                          | Tool managing source code history (e.g., Git)                           |

---

## Annex B — Key Concepts

### B.1 DO-178B Software Life Cycle

DO-178B defines an integrated set of processes, not a rigid waterfall. The key
processes are:

```
┌───────────────────────────────────────────────────────────────────┐
│                   Software Planning Process                       │
│  Produces: PSAC, SDP, SVP, SCM Plan, SQA Plan                     │
└──────────────────────┬────────────────────────────────────────────┘
                       │
           ┌───────────▼─────────────┐
           │  Requirements Process   │
           │  System Req → HLR → LLR │
           └───────────┬─────────────┘
                       │
           ┌───────────▼─────────────┐
           │    Design Process       │
           │  Architecture, SDD      │
           └───────────┬─────────────┘
                       │
           ┌───────────▼──────────────┐
           │    Coding Process        │
           │  Source code, @req tags  │
           └───────────┬──────────────┘
                       │
           ┌───────────▼─────────────┐
           │  Integration Process    │
           │  Build, link, target HW │
           └───────────┬─────────────┘
                       │
           ┌───────────▼──────────────┐
           │  Verification Process    │
           │  Reviews, testing, SC    │
           └──────────────────────────┘
```

Each process runs **concurrently** and feeds back into the others via
**transition criteria** and **problem reports**.

### B.2 Traceability

Traceability is the ability to follow every requirement **bidirectionally**
through the entire development chain:

```
  System Requirements
        ↕
  High-Level Requirements (HLR)
        ↕
  Low-Level Requirements (LLR)
        ↕
  Source Code  <>  Test Cases / Procedures
        ↕
  Test Results / Evidence
```

**Bidirectional** means:

- **Forward traceability** (top → down): every requirement has a corresponding
  implementation and test.
- **Backward traceability** (bottom → up): every line of code and test traces
  back to a requirement. Any code that doesn't is either a **derived
  requirement** or **dead code** (both require special handling).

In this module, traceability is achieved through `@req` and `@tc` Doxygen-style
tags embedded in the source code.

### B.3 Requirements Hierarchy

| Level                            | What It Describes           | Example from This Module                            |
| -------------------------------- | --------------------------- | --------------------------------------------------- |
| **System Requirement**           | What the _system_ must do   | "The system shall control a servo actuator via PWM" |
| **High-Level Requirement (HLR)** | What the _software_ must do | SRS-PWM-000: "PWM Servo Module Specification"       |
| **Low-Level Requirement (LLR)**  | _How_ the software does it  | LLR-PWM-022: "Clamp the input angle to [0, 180]"    |

HLRs are written in the **SRS**. LLRs are written in the **SDD** (this
document) and/or directly in the source code via `@req` tags.

### B.4 Structural Coverage

Structural coverage analysis confirms that the test suite exercises the code
adequately. The required level depends on the DAL:

- **Statement Coverage (SC)** — Every executable statement is reached at least
  once. Required for DAL-C and above.
- **Decision Coverage (DC)** — Every branch decision (e.g., `if` condition)
  evaluates to both `true` and `false`. Required for DAL-C and above.
- **Modified Condition/Decision Coverage (MC/DC)** — Every condition within a
  compound decision independently affects the outcome. Required for DAL-A and
  DAL-B only.

If coverage gaps are found, they must be justified with an analysis explaining
why the uncovered code is unreachable or benign (dead code analysis).

### B.5 Derived Requirements

A **derived requirement** is an LLR that does **not** trace upward to any
parent HLR or system requirement. It arises from the implementation itself
(e.g., "the pulse width must be saturated to the timer period" may not appear
in any system-level spec).

Per DO-178B §5.1.2, derived requirements must be:

1. **Identified** — tagged with `@derivedrq` in the source code.
2. **Reviewed** — the system safety team assesses whether the derived
   requirement introduces new failure modes.
3. **Fed back** — communicated to the system requirements process for potential
   inclusion in the SRS or HLR.

### B.6 Configuration Management (CM)

CM ensures that every version of every artifact (code, document, test case) is:

- **Identified** — unique CI/CSCI identifiers (e.g., CI-CM7-PWM).
- **Controlled** — changes go through a formal review/approval process.
- **Auditable** — the history of changes is preserved (via VCS tags/branches).
- **Baselined** — specific versions are frozen and associated with a release.

The **Software Configuration Index (SCI)** lists all items in a baseline and is
a required DO-178B deliverable.

### B.7 MISRA C:2012, Overview

MISRA C:2012 is a set of coding guidelines for the C language designed to
promote safety, security, and reliability. Rules are classified as:

| Category      | Compliance                                               |
| ------------- | -------------------------------------------------------- |
| **Mandatory** | Must always be followed; no deviations permitted         |
| **Required**  | Must be followed unless a formal deviation is documented |
| **Advisory**  | Recommended but deviations do not require documentation  |

Key rules applied in this module:

| Rule | Category | Summary                                                                             |
| ---- | -------- | ----------------------------------------------------------------------------------- |
| 3.1  | Required | No nested comments; prevents accidental comment-out                                 |
| 8.4  | Required | Every function must have a compatible prior declaration                             |
| 15.7 | Required | Every `if … else if` must end with a final `else`                                   |
| 17.7 | Required | The return value of a non-void function must be used or explicitly cast to `(void)` |

Deviations from Required rules must be formally documented in a **MISRA
Deviation Record** including: rule number, rationale, risk assessment, and
approval.

### B.8 Verification vs. Validation

| Term             | Question Answered                 | When                     |
| ---------------- | --------------------------------- | ------------------------ |
| **Verification** | "Did we build the product right?" | During development       |
| **Validation**   | "Did we build the right product?" | At system / flight level |

DO-178B focuses primarily on **verification**. Validation is covered by
ARP-4754A at the system level.

### B.9 Independence

For DAL-A and DAL-B, DO-178B requires **independence** in verification,
meaning the person who verifies (reviews, tests) the software must not be the
same person who developed it. For DAL-C and below, independence is recommended
but not mandatory.


## Annex C — MC/DC

Modified Condition/Decision Coverage (MC/DC) is a testing metric ensuring
that each atomic condition independently affects a decision's outcome. For a
decision with three Boolean conditions, exhaustive testing would require
`2^3 = 8` combinations; a minimal MC/DC set can be smaller while still showing
each condition's independent effect.

Example decision:

```c
if ((A && B) || C)
```

1. Truth table

| Case | A | B | C | Result |
| ---- | - | - | - | ------ |
| 1    | F | F | F | F      |
| 2    | F | F | T | T      |
| 3    | F | T | F | F      |
| 4    | F | T | T | T      |
| 5    | T | F | F | F      |
| 6    | T | F | T | T      |
| 7    | T | T | F | T      |
| 8    | T | T | T | T      |

2. Minimal MC/DC test set

| Test | A | B | C | Result | Independence shown |
| ---- | - | - | - | ------ | ------------------ |
| T1   | F | T | F | F      | A (with T2)        |
| T2   | T | T | F | T      | A (with T1), B (with T3) |
| T3   | T | F | F | F      | B (with T2), C (with T4) |
| T4   | T | F | T | T      | C (with T3)        |

Independence pairs:

- `A`: compare `T1` and `T2` with `B = T`, `C = F`
- `B`: compare `T2` and `T3` with `A = T`, `C = F`
- `C`: compare `T3` and `T4` with `A = T`, `B = F`
