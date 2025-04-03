// File: HystLag_C_Test.c
#include "HystLag_C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void run_c_hysteresis_test(const char* name,
                           HystDir direction,
                           float low, float high,
                           unsigned long lowLag, unsigned long highLag,
                           const unsigned long* times,
                           const float* values,
                           const int* expectedActive,
                           size_t count) {
    printf("Running C-style test: %s\n", name);

    char fileName[128];
	char sanitized[128];
	strncpy(sanitized, name, sizeof(sanitized));
    sanitized[sizeof(sanitized) - 1] = '\0';
    
    for (char* p = sanitized; *p; ++p) if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9'))) *p = '_';
	
	snprintf(fileName, sizeof(fileName), "%s.csv", sanitized);
    FILE* out = fopen(fileName, "w");
    if (!out) { perror("fopen"); exit(1); }

    fprintf(out, "# direction: %s\n", direction == HYST_DIR_UP ? "UP" : "DOWN");
    fprintf(out, "# lowThreshold: %.2f, highThreshold: %.2f\n", low, high);
    fprintf(out, "# lowLag: %lu ms, highLag: %lu ms\n", lowLag, highLag);
    fprintf(out, "time_ms,input,state,active\n");

    HystLag hyst;
    hystlag_init(&hyst, low, high, lowLag, highLag, direction);

    for (size_t i = 0; i < count; ++i) {
        unsigned long t = times[i];
        float v = values[i];

        int stabilized = hystlag_update(&hyst, v, t);
        int active = hystlag_is_active(&hyst);
        HystState state = hystlag_get_state(&hyst);

        const char* stateStr = (state == HYST_LOW) ? "LOW" : (state == HYST_HIGH) ? "HIGH" : "BETWEEN";

        printf("%5lu ms | input: %5.2f | state: %-8s | active: %d%s\n",
               t, v, stateStr, active, stabilized ? " (STABILIZED)" : "");

        fprintf(out, "%lu,%.2f,%s,%d\n", t, v, stateStr, active);

        if (expectedActive && expectedActive[i] != active) {
            fprintf(stderr, "Assertion failed at %lu ms: expected %d, got %d\n", t, expectedActive[i], active);
            exit(1);
        }
    }

    fclose(out);
    printf("✅ C test passed. Output written to: %s\n\n", fileName);
}


void testSimpleUP(void) {
    const unsigned long times[] = { 0, 200, 400, 600, 1100, 1300, 1900, 2500 };
    const float values[] = { 2.0f, 4.0f, 8.0f, 8.0f, 8.0f, 5.0f, 2.0f, 2.0f };
    const int expected[] = { 0, 0, 0, 0, 1, 1, 1, 0 };

    run_c_hysteresis_test(
        "UP direction, simple rise",
        HYST_DIR_UP,
        3.0f, 7.0f,
        500, 500,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testSimpleDown(void) {
    const unsigned long times[] = { 0, 300, 600, 1100, 1600, 2200 };
    const float values[] = { 8.0f, 6.0f, 2.0f, 2.0f, 5.0f, 7.5f };
    const int expected[] = { 0, 0, 0, 1, 1, 1 };

    run_c_hysteresis_test(
        "DOWN direction, simple fall",
        HYST_DIR_DOWN,
        3.0f, 7.0f,
        500, 500,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testInstantUpNoLag(void) {
    const unsigned long times[] = { 0, 1, 2 };
    const float values[] = { 2.0f, 8.0f, 2.0f };
    const int expected[] = { 0, 1, 0 };

    run_c_hysteresis_test(
        "UP, no lag, instant threshold crossing",
        HYST_DIR_UP,
        3.0f, 7.0f,
        0, 0,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testInstantDownNoLag(void) {
    const unsigned long times[] = { 0, 1, 2 };
    const float values[] = { 8.0f, 2.0f, 8.0f };
    const int expected[] = { 0, 1, 0 };

    run_c_hysteresis_test(
        "DOWN, no lag, instant threshold crossing",
        HYST_DIR_DOWN,
        3.0f, 7.0f,
        0, 0,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testHighLagUp(void) {
    const unsigned long times[] = { 0, 100, 900, 1100 };
    const float values[] = { 2.0f, 8.0f, 8.0f, 8.0f };
    const int expected[] = { 0, 0, 0, 1 };

    run_c_hysteresis_test(
        "UP, highLag delay",
        HYST_DIR_UP,
        3.0f, 7.0f,
        0, 1000,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testLowLagDown(void) {
    const unsigned long times[] = { 0, 100, 900, 1100 };
    const float values[] = { 8.0f, 2.0f, 2.0f, 2.0f };
    const int expected[] = { 0, 0, 0, 1 };

    run_c_hysteresis_test(
        "DOWN, lowLag delay",
        HYST_DIR_DOWN,
        3.0f, 7.0f,
        1000, 0,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testBetweenNeverActivates(void) {
    const unsigned long times[] = { 0, 100, 600, 1200 };
    const float values[] = { 5.0f, 5.5f, 6.0f, 6.5f };
    const int expected[] = { 0, 0, 0, 0 };

    run_c_hysteresis_test(
        "BETWEEN values never stabilize",
        HYST_DIR_UP,
        3.0f, 7.0f,
        500, 500,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testRapidToggling(void) {
    const unsigned long times[] = { 0, 100, 200, 300, 900 };
    const float values[] = { 2.0f, 8.0f, 2.0f, 8.0f, 8.0f };
    const int expected[] = { 0, 0, 0, 0, 1 };

    run_c_hysteresis_test(
        "UP, rapid toggling across thresholds",
        HYST_DIR_UP,
        3.0f, 7.0f,
        500, 500,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}

void testFullCoverageSequence(void) {
    const unsigned long times[] = {
        0, 100, 200, 400, 500, 600, 700,
        1000, 1100, 1200, 1600, 1700, 1800, 2100
    };

    const float values[] = {
        2.0f, 4.0f, 8.0f, 8.0f, 8.0f, 6.0f, 2.0f,
        2.0f, 6.5f, 8.0f, 8.0f, 3.0f, 2.0f, 2.0f
    };

    const int expected[] = {
        0, 0, 0, 0, 1,
        1, 1, 0,
        0, 0, 1,
        1, 1, 0
    };

    run_c_hysteresis_test(
        "Full_Sequence_Hysteresis_Test_C",
        HYST_DIR_UP,
        3.0f, 7.0f,
        300, 300,
        times, values, expected,
        sizeof(times) / sizeof(times[0])
    );
}





	
	int main() {
  testSimpleUP();
	testSimpleDown();
	testInstantUpNoLag();
	testInstantDownNoLag();
	testHighLagUp();
	testLowLagDown();
	testBetweenNeverActivates();
	testRapidToggling();
	testFullCoverageSequence();

    return 0;
}
