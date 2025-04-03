// File: HystLag.h
#pragma once
#ifndef HYSTLAG_H
#define HYSTLAG_H

class HystLag {
public:
    enum State { LOW = -1, BETWEEN = 0, HIGH = 1 };
    enum Direction { UP, DOWN };

    HystLag(float low, float high, unsigned long lowLag = 0, unsigned long highLag = 0, Direction direction = UP)
        : _low(low), _high(high), _lowLag(lowLag), _highLag(highLag),
          _direction(direction), _state(BETWEEN), _latchedState(BETWEEN), _stable(false), _lagStart(0) {
		if (low > high) {
			float tmp = _low;
			_low = _high;
			_high = tmp;
    		}
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
        if ((_state == HIGH && (_highLag == 0 || dt >= _highLag)) ||
            (_state == LOW  && (_lowLag == 0  || dt >= _lowLag))) {
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
        return (_direction == UP)   ? (_latchedState == HIGH)
             : (_direction == DOWN) ? (_latchedState == LOW)
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
        if (value > _high) return HIGH;
        if (value < _low)  return LOW;
        return BETWEEN;
    }
};

#endif //HYSTLAG_H
