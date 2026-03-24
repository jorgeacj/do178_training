# PSAC - Plan for Software Aspects of Certification

## Purpose
This document defines the plan for the software aspects of certification for the flap control system. 

## System Overview
The system is a motor control system for a flap system on an aircraft. 

## Software Overview
The software is responsible for controlling the flap motor based on the reading of the message coming from the CAN communication. It must run real-time and must be deterministic. It is going to run on a STM32H7 microcontroller, where the M4 core is used for the application and the M7 core is used for the motor control. 

## Certification Considerations
This software is classified with Design Assurance Level (DAL) A, as per DO-178C. The loss of a motor control system of a aircraft flap can lead to a catastrophic event, causing the loss of the aircraft and several deaths.

## Software Lifecycle
The implementation will follow a V-model approach. First phase will be the System Requirements elicitation, followed by the Software Requirements elicitation, Software Architecture design, Software Detailed Design, Software Implementation, Software Integration and Software Verification. 

## Software Lifecycle Data
The list of documents that will be produced during the software lifecycle are:
- Plan for Software Aspects of Certification (PSAC)
- Software Requirements Document (SRD)
- Software Architecture Document (SAD)
- Software Development Plan (SDP)
- Software Design Document (SDD)
- Software Verification Plan (SVP)
- Software Verification Report (SVR)
- Software Configuration Management Plan (SCMP)
- Software Configuration Management Report (SCMR)
- Software Quality Assurance Plan (SQAP)
- Software Quality Assurance Report (SQAR)
- Software Test Plan (STP)
- Software Test Report (STR)
- Software Traceability Matrix (STM)
- Software Change Request (SCR)
- Software Problems Reports (SPR)
- Software Accomplishment Summary (SAS)
- Source Code
- Test Results
- Traceability Matrix

## Schedule
This plan is supposed to be completed by the end of the year 2026.

## Additional Considerations
The software will be developed using C language and will be compiled using GCC compiler. The software will be implemented using FreeRTOS and HAL libraries, whenever possible.