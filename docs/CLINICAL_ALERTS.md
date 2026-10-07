# Clinical Alert Hierarchy & Acoustic Profiles (IEC 60601-1-8 Alignment)

This document establishes the clinical alarm arbitration rules, priority matrix, visual cues, and acoustic buzzer cadences for the bedside monitor.

---

## 1. Clinical Alarm Priority Matrix

| Clinical Condition | State Enum | IEC 60601-1-8 Priority | Visual Indicator | Buzzer Acoustic Cadence | Protocol / Action |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Nurse Emergency Call** | `ICU_STATE_NURSE_CALL` | **HIGH (Critical)** | Flashing Red LED | Rapid Urgent Pulse (200ms ON / 100ms OFF) | Immediate bedside response required |
| **Patient Fall Detected** | `ICU_STATE_PATIENT_FALL` | **HIGH (Critical)** | Steady Red LED | Continuous High-Pitch Siren | Emergency code fall intervention |
| **Bed Exit Attempt** | `ICU_STATE_BED_EXIT_ATTEMPT`| **MEDIUM (Warning)**| Steady Amber LED | Intermittent Beep (500ms ON / 500ms OFF) | Assist patient back to safe center |
| **Abnormal Bed Incline** | `ICU_STATE_ABNORMAL_BED_TILT`| **MEDIUM (Warning)**| Steady Amber LED | Intermittent Beep (500ms ON / 500ms OFF) | Adjust Fowler angle within $0^\circ - 60^\circ$ |
| **Bed Vacant** | `ICU_STATE_BED_VACANT` | **LOW (Info)** | Slow Pulsing Green | Silent | Awaiting patient admission |
| **Normal Monitoring** | `ICU_STATE_NORMAL_MONITORING`| **NORMAL (Safe)** | Steady Green LED | Silent | Routine bedside observation |

---

## 2. Priority Resolution Rules
In multi-fault conditions, the arbiter follows deterministic precedence:
1. `NURSE_CALL` takes absolute precedence over all other conditions.
2. `PATIENT_FALL` takes precedence over bed exit or tilt warnings.
3. `BED_EXIT_ATTEMPT` takes precedence over `ABNORMAL_BED_TILT`.
4. Alarms cannot be masked or cleared until physical resolution or nurse acknowledgment.
