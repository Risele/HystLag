// File: HystLag.h
#pragma once
#ifndef HYSTLAG_H
#define HYSTLAG_H

class HystLag {
public:
    enum State { HYST_LOW = -1, BETWEEN = 0, HYST_HIGH = 1 };
    enum Direction { UP, DOWN };

    HystLag(float low, float high, unsigned long lowLag = 0, unsigned long highLag = 0, Direction direction = UP){
        init(low, high, lowLag, highLag, direction);  
    }
      void reset() {

        _state = BETWEEN;
        _latchedState = BETWEEN;
        _stable = false;
        _lagStart = 0;
    }
    void init(float low, float high, unsigned long lowLag, unsigned long highLag, Direction direction){
        if (low>high){
            _low = high;
            _high = low;
        }
        else {
            _low = low;
            _high = high;
        }
        
        _low = low;
        _high = high;
        _lowLag = lowLag;
        _highLag = highLag;
        _direction = direction;
        reset();

    }
   bool update(float value, unsigned long currentTime) {
    State newState = calculateState(value);

    if (newState == BETWEEN) {
		_stable = false;///addded from C
        return false;
    }

    bool stateChanged = (newState != _state);
    if (stateChanged) {
        _state = newState;
        _lagStart = currentTime;
        _stable = false;
    }

    if (!_stable) {
        unsigned long dt = currentTime - _lagStart;
        if ((_state == HYST_HIGH && (_highLag == 0 || dt >= _highLag)) ||
            (_state == HYST_LOW  && (_lowLag == 0  || dt >= _lowLag))) {
            _stable = true;
            _latchedState = _state;
            onStabilize(_state);
            return true;
        }
    }

    return false;
}


    virtual void onStabilize(State stabilizedState) {
        // Optional override
    }

    bool isActive() const {
        return (_direction == UP)   ? (_latchedState == HYST_HIGH)
             : (_direction == DOWN) ? (_latchedState == HYST_LOW)
             : false;
    }

    State getState() const { return _state; }
    bool isStable() const { return _stable; }
    void setDirection(Direction dir) { _direction = dir; }

private:
    float _low, _high;
    unsigned long _lowLag, _highLag;
    unsigned long _lagStart;
    Direction _direction;
    State _state;
    State _latchedState;
    bool _stable;

    State calculateState(float value) const {
        if (value > _high) return HYST_HIGH;
        if (value < _low)  return HYST_LOW;
        return BETWEEN;
    }
};

#endif //HYSTLAG_H
