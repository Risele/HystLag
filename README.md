# HystLag
Hysteresis with set/reset time lag

A lightweight, test-driven hysteresis state handler for embedded systems and simulations.
Supports both:
- ✅ C++ class-based version
- ✅ Pure C struct-based version (function style)

## Features
- Configurable upper/lower thresholds
- Time lag for stabilization (highLag, lowLag)
- Direction control (UP or DOWN triggering)
---
## Usage

The very minumum example:
### C style
```C
#include "HystLag_C.h"

unsigned long millis = 0; //time counter, replace with your own.
float read_sensor() {
    return (millis%1000) < 250 ? 2.0f : 8.0f;
}
int main(void) {
    HystLag hyst;
    HystLag_init(&hyst, 3.0f, 7.0f, 50, 50, HYST_DIR_UP);

    while (1) {
        float input = read_sensor();
        hystlag_update(&hyst, input, millis);
        if (hystlag_is_active(&hyst)) {
            // Do something: e.g. turn on relay, LED, fan, etc.
        } else {
            // Deactivate output
        }
        millis += 10; // Simulate time step 
    }
    return 0;
}
```
### C++ style:
```Cpp
#include "HystLag.h"

unsigned long millis = 0; //time counter, replace with your own.
float read_sensor() {
    return (millis%1000) < 250 ? 2.0f : 8.0f;
}
int main() {
    HystLag hyst(3.0f, 7.0f, 50, 50, HystLag::UP);
    while (true) {
        float input = readSensor();
        hyst.update(input, millis);
        if (hyst.isActive()) {
          // Do something: e.g. turn on relay, LED, fan, etc.
        } else {
           // Deactivate output
        }
        millis += 10;  // Simulate time step 
    }
    return 0;
}
```
## 📦 Files

```text
HystLag.h         → C++ interface & implementation
HystLag_C.h/.c    → C-compatible interface and logic

/tests/
  HysteresisTest.h   → C++ test suite (CSV outputs + assertions)
  Hysteresis_C_Test.c → C test suite with matching logic
 
```

---

## 🧪 Run Tests

### C++:
```bash
g++ -std=c++11 -o HystLag_Cpp_Test HystLag_Cpp_Test.cpp
HystLag_Cpp_Test.exe
pause 
```

### C:
```bash
gcc HystLag_C.c HystLag_C_Test.c -o HystLag_C_Test
HystLag_C_Test.exe
pause
```

Outputs CSV files like `Full_Sequence_Hysteresis_Test_C.csv` for graphing.

---

## 📈 CSV Format
```
time_ms,input,state,active
0,2.0,LOW,0
...
```

Each file starts with parameters used:
```
# direction: UP
# lowThreshold: 3.00, highThreshold: 7.00
# lowLag: 300 ms, highLag: 300 ms
```
Transitions between different input values, states and activation are emphasized for plotting by placing additional point just before the transition.

---
