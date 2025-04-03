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
g++ -std=c++11 tests/main.cpp -o test_cpp
./test_cpp
pause 
```

### C:
```bash
g++ -std=c++11 -o HystLag_CPP HystLag_Cpp_Test.cpp
HystLag_CPP.exe
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

---
