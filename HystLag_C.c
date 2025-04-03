
// File: HystLag_C.c
#include "HystLag_C.h"

static HystState calculate_state(const HystLag* h, float value) {
    if (value > h->high) return HYST_HIGH;
    if (value < h->low)  return HYST_LOW;
    return HYST_BETWEEN;
}

void hystlag_init(HystLag* h, float low, float high, unsigned long lowLag, unsigned long highLag, HystDir dir) {
    if (low > high) {
        float tmp = low;
        low = high;
	high = tmp;
    }
    h->low = low;
    h->high = high;
    h->lowLag = lowLag;
    h->highLag = highLag;
    h->direction = dir;
    h->state = HYST_BETWEEN;
    h->latchedState = HYST_BETWEEN;
    h->stable = false;
    h->lagStart = false;
}

int hystlag_update(HystLag* h, float value, unsigned long currentTime) {
    HystState newState = calculate_state(h, value);

    if (newState == HYST_BETWEEN) {
		h->stable = false; 
        return false;
    }

    if (newState != h->state) {
        h->state = newState;
        h->lagStart = currentTime;
        h->stable = false;
    }

    if (!h->stable) {
        unsigned long dt = currentTime - h->lagStart;
        if ((h->state == HYST_HIGH && (h->highLag == 0 || dt >= h->highLag)) ||
            (h->state == HYST_LOW && (h->lowLag == 0 || dt >= h->lowLag))) {
            h->stable = true;
            h->latchedState = h->state;
            return true;
        }
    }

    return false;
}


int hystlag_is_active(const HystLag* h) {
    return (h->direction == HYST_DIR_UP)   ? (h->latchedState == HYST_HIGH)
         : (h->direction == HYST_DIR_DOWN) ? (h->latchedState == HYST_LOW)
         : false;
}

HystState hystlag_get_state(const HystLag* h) {
    return h->state;
}
