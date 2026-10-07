# Hardware Wiring, Pinout & Schematic Specification

This guide documents the physical connections and Bill of Materials (BOM) for deploying the system on the **STM32 Black Pill (STM32F401CCU6 / STM32F411CEU6)**.

---

## 1. STM32 Black Pill Pinout Table

| Peripheral | Signal Pin | Black Pill GPIO | Function / Details |
| :--- | :--- | :--- | :--- |
| **MPU6050 IMU** | `SCL` | **PB8** | I2C1 Clock (400 kHz Fast-Mode) |
| **MPU6050 IMU** | `SDA` | **PB9** | I2C1 Data (400 kHz Fast-Mode) |
| **SSD1306 OLED** | `SCL` | **PB8** | Shared I2C1 Bus (`0x3C`) |
| **SSD1306 OLED** | `SDA` | **PB9** | Shared I2C1 Bus (`0x3C`) |
| **HC-SR04 Ultrasonic** | `TRIG` | **PA1** | 10 $\mu$s Trigger Output Pulse |
| **HC-SR04 Ultrasonic** | `ECHO` | **PA2** | Timer Input Capture (5V tolerant / divider) |
| **LDR Sensor** | `A0` | **PA0** | ADC1 Channel 0 (12-bit, 0 - 3.3V) |
| **Left IR Sensor** | `OUT` | **PA3** | Digital Input (Active LOW rail barrier) |
| **Right IR Sensor** | `OUT` | **PA4** | Digital Input (Active LOW rail barrier) |
| **Nurse Call Switch** | Terminal 1 | **PA5** | EXTI5 External Interrupt (Pull-up) |
| **Active Buzzer** | Positive | **PB0** | TIM3 PWM / GPIO Alarm Output |
| **Green LED (Safe)** | Anode | **PB1** | Normal Clinical Status |
| **Yellow LED (Warn)** | Anode | **PB12** | Advisory / Warning Alert |
| **Red LED (Critical)**| Anode | **PB13** | Emergency Alert (Fall / Nurse Call) |
| **Onboard Heartbeat** | LED | **PC13** | Built-in Green LED (Active LOW) |
| **Serial Debug** | `TX` | **PA9** | USART1 115200 Baud Stream |
| **Serial Debug** | `RX` | **PA10** | USART1 115200 Baud Console |

---

## 2. Schematic Diagram (ASCII Architecture)

```
                     +---------------------------------------+
                     |   STM32 Black Pill (Cortex-M4 84MHz)  |
                     |                                       |
    [PB8 (I2C1_SCL)] +---------+----------------+            |
    [PB9 (I2C1_SDA)] +-------+ |                |            |
                     |       | |                |            |
    [PA1 (TRIG)] ----+-------|-|----------------|-------+    |
    [PA2 (ECHO)] ----+-------|-|----------------|-----+ |    |
                     |       | |                |     | |    |
    [PA0 (ADC1_0)] --+-------|-|----------------|---[LDR Divider]
                     |       | |                |            |
    [PA3 (IR_L)] ----+-------|-|----------------|-----[IR Sensor L]
    [PA4 (IR_R)] ----+-------|-|----------------|-----[IR Sensor R]
                     |       | |                |            |
    [PA5 (EXTI5)] ---+-------|-|----------------|---[Nurse Pushbutton]--- GND
                     |       | |                |            |
    [PB0 (BUZZER)] --+-------|-|----------------|---[Active Buzzer]------ GND
    [PB1 (LED_G)] ---+-[330R]|-|-[>| (Green)]---|------------------------ GND
    [PB12(LED_Y)] ---+-[330R]|-|-[>| (Amber)]---|------------------------ GND
    [PB13(LED_R)] ---+-[330R]|-|-[>| (Red)]-----|------------------------ GND
                     +-------|-|----------------+------------+
                             | |                |
                             | |                +--------+
                             | |                         |
                       +-----+---+                 +-----+---+
                       | MPU6050 |                 | SSD1306 |
                       | 6-DoF   |                 | OLED    |
                       +---------+                 +---------+
```

---

## 3. Bill of Materials (BOM)

| Item | Component | Quantity | Approximate Cost (USD) |
| :--- | :--- | :--- | :--- |
| 1 | STM32F401CCU6 Black Pill Board | 1 | $3.80 |
| 2 | MPU6050 6-Axis Accelerometer & Gyroscope | 1 | $1.70 |
| 3 | HC-SR04 Ultrasonic Distance Sensor | 1 | $1.20 |
| 4 | 0.96" 128x64 I2C OLED Display (SSD1306) | 1 | $2.90 |
| 5 | LDR Light-Dependent Resistor Module | 1 | $0.60 |
| 6 | Active Low IR Obstacle Sensor Modules | 2 | $1.40 |
| 7 | Emergency Momentary Push Button | 1 | $0.40 |
| 8 | 5V Active Piezo Buzzer | 1 | $0.50 |
| 9 | 5mm LEDs (Green, Yellow, Red) & 330 $\Omega$ Resistors | 3 pairs | $0.50 |
| **Total** | | | **~$13.00** |
