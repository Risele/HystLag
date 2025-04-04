// File: HystLag_Cpp_Test.h
#pragma once

#include "HystLag.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <iomanip>
#include <fstream>

inline std::string stateToStr(HystLag::State state) {
    switch (state) {
        case HystLag::LOW: return "LOW";
        case HystLag::BETWEEN: return "BETWEEN";
        case HystLag::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}
inline void testCase(const std::string& name,
              HystLag::Direction direction,
              float low, float high,
              unsigned long offLag, unsigned long onLag,
              const std::vector<std::pair<unsigned long, float>>& samples,
              const std::vector<bool>& expectedActives)
{
    std::cout << "Running test: " << name << std::endl;

    std::string fileName = name;
    for (char& c : fileName) {
        if (!std::isalnum(c)) c = '_';
    }
    fileName += ".csv";
    std::ofstream outFile(fileName);
	outFile << "# direction: " << (direction == HystLag::UP ? "UP" : "DOWN") << "\n";
    outFile << "# lowThreshold: " << low << ", highThreshold: " << high << "\n";
    outFile << "# offLag: " << offLag << " ms, onLag: " << onLag << " ms\n";
    outFile << "time_ms,input,state,active\n";
    outFile << "time_ms,input,state,active\n";

    HystLag hyst(low, high, offLag, onLag, direction);

    HystLag::State lastState = HystLag::BETWEEN;
    bool lastActive = false;

    for (size_t i = 0; i < samples.size(); ++i) {
        unsigned long t = samples[i].first;
        float v = samples[i].second;

        bool stabilized = hyst.update(v, t);
        bool active = hyst.isActive();
        HystLag::State state = hyst.getState();
        std::string stateStr = stateToStr(state);

        std::cout << std::setw(5) << t << " ms | input: "
                  << std::setw(5) << v
                  << " | state: " << std::setw(8) << stateStr
                  << " | active: " << std::boolalpha << active
                  << (stabilized ? " (STABILIZED)" : "") << std::endl;

        // Intermediate point between time steps
        if (i > 0) {
            unsigned long prevT = samples[i - 1].first;
            float prevV = samples[i - 1].second;
            unsigned long midT = (prevT + t) / 2;
            outFile << midT << "," << prevV << "," << stateToStr(lastState) << "," << (lastActive ? 1 : 0) << "\n";
        }

        // Extra point if state or active changed
        if (i > 0 && (state != lastState || active != lastActive)) {
            unsigned long prevT = samples[i - 1].first;
            float prevV = samples[i - 1].second;
            unsigned long transT = (prevT + t) / 2;
            outFile << transT - 1 << "," << prevV << "," << stateToStr(lastState) << "," << (lastActive ? 1 : 0) << "\n";
            outFile << transT     << "," << v     << "," << stateStr            << "," << (active ? 1 : 0) << "\n";
        }

        outFile << t << "," << v << "," << stateStr << "," << (active ? 1 : 0) << "\n";

        lastState = state;
        lastActive = active;

        if (i < expectedActives.size()) {
            assert(active == expectedActives[i]);
        }
    }

    outFile.close();
    std::cout << " + Test passed. Output written to: " << fileName << "\n" << std::endl;
}

// === Additional test coverage ===


inline void testSimpleUP(){
	testCase("UP direction, simple rise",
             HystLag::UP,
             3.0f, 7.0f, 500, 500,
             {{0, 2.0f}, {200, 4.0f}, {400, 8.0f}, {600, 8.0f}, {1100, 8.0f}, {1300, 5.0f}, {1900, 2.0f}, {2500, 2.0f}},
             {false, false, false, false, true, true, true, false});
}
inline void testSimpleDown(){
	
testCase("DOWN direction, simple fall",
             HystLag::DOWN,
             3.0f, 7.0f, 500, 500,
             {{0, 8.0f}, {300, 6.0f}, {600, 2.0f}, {1100, 2.0f}, {1600, 5.0f}, {2200, 7.5f}},
             {false, false, false, true, true, true});

}
// 1. No lag, UP direction, instant toggle
inline void testInstantUpNoLag() {
    testCase("UP, no lag, instant threshold crossing",
        HystLag::UP, 3.0f, 7.0f, 0, 0,
        {{0, 2.0f}, {1, 8.0f}, {2, 2.0f}},
        {false, true, false});
}

// 2. No lag, DOWN direction, instant toggle
inline void testInstantDownNoLag() {
    testCase("DOWN, no lag, instant threshold crossing",
        HystLag::DOWN, 3.0f, 7.0f, 0, 0,
        {{0, 8.0f}, {1, 2.0f}, {2, 8.0f}},
        {false, true, false});
}

// 3. Long onLag, UP direction
inline void testOnLagUp() {
    testCase("UP, onLag delay",
        HystLag::UP, 3.0f, 7.0f, 0, 1000,
        {{0, 2.0f}, {100, 8.0f}, {900, 8.0f}, {1100, 8.0f}},
        {false, false, false, true});
}

// 4. Long offLag, DOWN direction
inline void testOffLagDown() {
    testCase("DOWN, offLag delay",
        HystLag::DOWN, 3.0f, 7.0f, 1000, 0,
        {{0, 8.0f}, {100, 2.0f}, {900, 2.0f}, {1100, 2.0f}},
        {false, false, false, true});
}

// 5. BETWEEN never stabilizes
inline void testBetweenNeverActivates() {
    testCase("BETWEEN values never stabilize",
        HystLag::UP, 3.0f, 7.0f, 500, 500,
        {{0, 5.0f}, {100, 5.5f}, {600, 6.0f}, {1200, 6.5f}},
        {false, false, false, false});
}

// 6. Rapid toggle across thresholds (debounce test)
inline void testRapidToggling() {
    testCase("UP, rapid toggling across thresholds",
        HystLag::UP, 3.0f, 7.0f, 500, 500,
        {{0, 2.0f}, {100, 8.0f}, {200, 2.0f}, {300, 8.0f}, {900, 8.0f}},
        {false, false, false, false, true});
}

// 7. Comprehensive multi-phase test covering UP and DOWN transitions, delays, and noise
inline void testFullCoverageSequence() {
    testCase("Full Sequence Hysteresis Test",
        HystLag::UP, 3.0f, 7.0f, 300, 300,
        {
            {0, 2.0f},      // LOW
            {100, 4.0f},    // BETWEEN
            {200, 8.0f},    // HIGH begins
            {400, 8.0f},    // still not stable
            {500, 8.0f},    // stable HIGH -> active
            {600, 6.0f},    // BETWEEN
            {700, 2.0f},    // LOW begins
            {1000, 2.0f},   // stable LOW -> inactive
            {1100, 6.5f},   // BETWEEN
            {1200, 8.0f},   // HIGH begins again
            {1600, 8.0f},   // stable HIGH -> active
            {1700, 3.0f},   // BETWEEN
            {1800, 2.0f},   // LOW begins
            {2100, 2.0f},   // stable LOW -> inactive
        },
        {
            false, false, false, false, true,
            true, true, false,
            false, false, true,
            true, true, false
        }
    );
}