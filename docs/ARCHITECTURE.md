# System Architecture & Clinical Engineering Specification

The **Smart Medical ICU Safety & Patient Monitoring System** is an embedded medical bedside guardian powered by the **STM32 Black Pill (ARM Cortex-M4)**. Designed for Intensive Care Units (ICU), geriatric care, and recovery wards, it continuously monitors patient posture, bed elevation (Fowler's position), bed occupancy, boundary breaches, impact falls, emergency nurse calls, and ambient room lighting.

---

## 1. High-Level System Architecture

```mermaid
flowchart TD
    subgraph Sensors ["Multi-Sensor Acquisition Layer"]
        A[MPU6050 6-Axis IMU\nI2C: 400 kHz] -->|3-Axis Accel & Gyro| D[Posture & Incline Engine]
        B[HC-SR04 Ultrasonic Sensor\nTimer Pulse Capture] -->|Mattress Distance cm| E[Bed Occupancy Debouncer]
        C[LDR Ambient Light Sensor\n12-bit ADC1 Ch0] -->|Raw Lux Level| F[Ambient Light Adaptor]
        G[Dual IR Boundary Sensors\nDigital Rail Barriers] -->|Left/Right Trip| H[Boundary Breach Detector]
        I[Emergency Nurse Switch\nEXTI External Interrupt] -->|Instant Trigger| J[Emergency Call Latch]
    end

    subgraph Processing ["Core Clinical Decision Layer"]
        D & E & H & J --> K[Finite-State Machine FSM\nIEC 60601-1-8 Priority Arbiter]
        K --> L[Clinical Alert Matrix]
    end

    subgraph Actuation ["Actuator & Telemetry Layer"]
        L --> M[Tri-Color Clinical LEDs\nGreen / Amber / Red]
        L --> N[Active Buzzer Alarm\nCadence Controller]
        L --> O[SSD1306 OLED Display\nReal-Time Bedside UI]
        L --> P[USART1 Telemetry Gateway\n115200 Baud JSON / ASCII]
    end
```

---

## 2. Finite-State Machine (FSM) Diagram

```mermaid
stateDiagram-v2
    [*] --> BED_VACANT : System Boot

    BED_VACANT --> NORMAL_MONITORING : Patient In Bed (Dist: 10-85cm)
    NORMAL_MONITORING --> BED_VACANT : Patient Discharged / Vacant
    
    NORMAL_MONITORING --> BED_EXIT_ATTEMPT : IR Rail Broken / Shifting
    BED_EXIT_ATTEMPT --> NORMAL_MONITORING : Patient Returns to Center
    
    NORMAL_MONITORING --> ABNORMAL_BED_TILT : Incline > 60 deg
    ABNORMAL_BED_TILT --> NORMAL_MONITORING : Bed Lowered to Safe Angle
    
    NORMAL_MONITORING --> PATIENT_FALL : Shock > 2.6g + Bed Vacant
    BED_EXIT_ATTEMPT --> PATIENT_FALL : Shock > 2.6g + Bed Vacant
    
    NORMAL_MONITORING --> NURSE_CALL : Emergency Button Pressed
    BED_EXIT_ATTEMPT --> NURSE_CALL : Emergency Button Pressed
    PATIENT_FALL --> NURSE_CALL : Emergency Button Pressed
    
    NURSE_CALL --> NORMAL_MONITORING : Staff Acknowledged / Reset
    PATIENT_FALL --> NORMAL_MONITORING : Staff Attended / Reset
```

---

## 3. Mathematical & Algorithmic Derivations

### 3.1 Posture & Bed Incline Calculation
Using the gravitational acceleration vector from the MPU6050 accelerometer:
$$\vec{A} = [a_x, a_y, a_z]$$
$$\text{Total Acceleration Magnitude} \quad |\vec{A}| = \sqrt{a_x^2 + a_y^2 + a_z^2}$$

The bed head incline (Fowler's position angle $\theta_{\text{pitch}}$) and lateral tilt ($\phi_{\text{roll}}$) are extracted via:
$$\theta_{\text{pitch}} = \text{atan2}(a_y, a_z) \cdot \frac{180^\circ}{\pi}$$
$$\phi_{\text{roll}} = \text{atan2}(a_x, a_z) \cdot \frac{180^\circ}{\pi}$$

*Clinical Safe Zone: $0^\circ \le \theta_{\text{pitch}} \le 60^\circ$. Angles $> 60^\circ$ present aspiration or circulatory hazards and trigger an advisory alert.*

### 3.2 Impact Fall Detection Algorithm
A fall event is distinguished from normal shifting by a dual-stage trigger:
1. **Kinematic Impact**: Peak dynamic acceleration exceeding the critical shock threshold:
   $$|\vec{A}| \ge 2.60g$$
2. **Mattress Disconnect**: Ultrasonic sensor reading exceeds bed boundary ($d > 85\text{ cm}$) or boundary IR sensors break simultaneously.

### 3.3 Bed Occupancy Debounce Filter
To prevent false vacancy alarms caused by patient breathing, coughing, or rolling over, mattress proximity is debounced across $N = 5$ consecutive sample cycles ($250\text{ ms}$).
