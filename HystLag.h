// File: HystLag.h
#pragma once
#ifndef HYSTLAG_H
#define HYSTLAG_H

class HystLag {
public:
    enum State { HYST_LOW = -1, BETWEEN = 0, HYST_HIGH = 1 };
    enum Direction { UP, DOWN };
	HystLag(){
        _low=0;
        _high=0;
        _onLag=0;
        _offLag=0;
        _direction=UP;
        reset();
    }
    HystLag(float low, float high, unsigned long offLag = 0, unsigned long onLag = 0, Direction direction = UP){
        init(low, high, offLag, onLag, direction);  
    }
      void reset() {

        _state = BETWEEN;
        _latchedState = BETWEEN;
        _stable = false;
        _lagStart = 0;
    }
    void init(float low, float high, unsigned long offLag, unsigned long onLag, Direction direction){
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
        _offLag = offLag;
        _onLag = onLag;
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
        if ((_state == HYST_HIGH && (_onLag == 0 || dt >= _onLag)) ||
            (_state == HYST_LOW  && (_offLag == 0  || dt >= _offLag))) {
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
    unsigned long _offLag, _onLag;
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
