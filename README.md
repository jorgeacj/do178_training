# do178_training
This repository contains a project for training embedded software development in comply with DO178C.

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
- [ ] Define coding standard (MISRA-oriented subset + project rules) + deviation process template
- [ ] Enforce “no dynamic allocation after init” rule (malloc/free banned) + build check
- [ ] Enforce “no HAL calls from application” (BSP wrapper-only access)
 
### BSP / platform framework (HAL/LL strategy)
- [ ] Implement BSP wrappers: GPIO, timebase, timer, UART, CAN, watchdog, EXTI
- [ ] Define and document ISR policy (minimal ISR, defer work, shared data rules)
- [ ] Set up scheduler/superloop structure with fixed-rate tasks
 
### Timing / determinism
- [ ] Implement monotonic time base + overflow behavior
- [ ] Implement periodic execution model (e.g., 1 ms tick + 10 ms control loop)
- [ ] Measure and document control-loop jitter / step pulse timing jitter (bounded jitter requirement)
 
### Stepper motor control (driver layer)
- [ ] Implement STEP pulse generation using timer-based mechanism
- [ ] Implement DIR/EN control and safe disable behavior
- [ ] Implement speed limiting and acceleration limiting support in driver/control boundary
 
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