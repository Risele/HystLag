
// File: HystLag_C.c
#include "HystLag_C.h"

static HystState calculate_state(const HystLag* h, float value) {
    if (value > h->high) return HYST_HIGH;
    if (value < h->low)  return HYST_LOW;
    return HYST_BETWEEN;
}

void hystlag_init(HystLag* h, float low, float high, unsigned long offLag, unsigned long onLag, HystDir dir) {
    if (low > high) {
        h->high = low;
        h->low = high;
    } else{
        h->low = low;
        h->high = high;
    }
       
    h->offLag = offLag;
    h->onLag = onLag;
    h->direction = dir;
    hystlag_reset(h);
}

void hystlag_reset(HystLag* h) {
    h->state = HYST_BETWEEN;
    h->latchedState = HYST_BETWEEN;
    h->stable = false;
    h->lagStart = 0;
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
        if ((h->state == HYST_HIGH && (h->onLag == 0 || dt >= h->onLag)) ||
            (h->state == HYST_LOW && (h->offLag == 0 || dt >= h->offLag))) {
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
