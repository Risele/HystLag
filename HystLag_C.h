// File: HystLag_C.h
#pragma once
#ifndef HYSTLAG_H
#define HYSTLAG_H
#ifdef __cplusplus
extern "C" {
#endif
#ifndef true
	#define true 1
	#define false 0
#endif
// Equivalent states and direction enum
typedef enum {
    HYST_LOW = -1,
    HYST_BETWEEN = 0,
    HYST_HIGH = 1
} HystState;

typedef enum {
    HYST_DIR_UP,
    HYST_DIR_DOWN
} HystDir;

// Struct-based Hysteresis context
typedef struct {
    float low;
    float high;
    unsigned long offLag;
    unsigned long onLag;
    unsigned long lagStart;

    HystDir direction;
    HystState state;
    HystState latchedState;
    int stable;
} HystLag;

// Initialization
void hystlag_init(HystLag* h, float low, float high, unsigned long offLag, unsigned long onLag, HystDir dir);
//Reset the current state to between
void hystlag_reset(HystLag* h);
// Update with input value and timestamp (in ms)
int hystlag_update(HystLag* h, float value, unsigned long currentTime);

// Get current stable output state
int hystlag_is_active(const HystLag* h);

// Get current raw state
HystState hystlag_get_state(const HystLag* h);

#ifdef __cplusplus
}
#endif


#endif //HYSTLAG_H