#if !defined(_MOVUINO_BUTTON_)
#define _MOVUINO_BUTTON_

#include <Arduino.h>

class Button
{
private:
    unsigned int _pin;
    bool _isPullUp = true; // default (case Movuino)
    int _btnState;         // the current reading from the input pin
    int _lastState = LOW;  // the previous reading from the input pin

    unsigned long _lastDebounceTime = 0;
    unsigned long _debounceDelay;

    unsigned int _delayReClick = 400;
    unsigned int _maxReClick = 3;
    unsigned long _lastTimesOnPress[3];
    unsigned long _lastTimeOnPress;
    bool _isJustReleased = false;
    unsigned long _lastTimeOnRelease;

    int _nValidClicked = 0;
    int getNumberOfClicked(unsigned long *timesArray_);

public:
    Button(int pin_);
    Button(int pin_, bool isPullUp_);
    ~Button(){};

    void begin();
    void update();

    void setDebounceInterval(int debounceInterval_) { this->_debounceDelay = debounceInterval_; };
    void setMaxReClick(int maxReClick_) { this->_maxReClick = constrain(maxReClick_, 1, 3); };

    bool isPressed() { return _btnState; };
    bool isReleased() { return _isJustReleased; };
    bool isClicked(); // (on press)
    bool isDoubleClicked();
    bool isTripleClicked();
    unsigned int timeHold();          // current time holding the button
    unsigned int timeHoldOnRelease(); // last duration holding the button

    bool isOnClickRelease();       // ! Blocking
    bool isOnDoubleClickRelease(); // ! Blocking
    bool isOnTripleClickRelease(); // ! Blocking
};

Button::Button(int pin_)
{
    this->_pin = pin_;
}

Button::Button(int pin_, bool isPullUp_)
{
    this->_pin = pin_;
    this->_isPullUp = isPullUp_;
}

void Button::begin()
{
    pinMode(this->_pin, INPUT);

    // reset
    for (int i = 0; i < this->_maxReClick; i++)
    {
        this->_lastTimesOnPress[i] = 0;
    }

    // first read
    this->_btnState = digitalRead(this->_pin);
    if (this->_isPullUp)
        this->_btnState = !this->_btnState; // reverse if pull up (default)
}

void Button::update()
{
    // Read and debounce button state
    int state_ = digitalRead(this->_pin);
    if (this->_isPullUp)
        state_ = !state_; // reverse if pull up (default)

    if (state_ != this->_lastState)
        this->_lastDebounceTime = millis(); // start debounce timer

    this->_isJustReleased = false;
    if (millis() - this->_lastDebounceTime > this->_debounceDelay)
    {
        if (state_ != this->_btnState)
        {
            if (state_)
            {
                // ON PRESS
                for (int i = this->_maxReClick; i > 0; i--)
                {
                    this->_lastTimesOnPress[i] = this->_lastTimesOnPress[i - 1]; // shift array
                }
                this->_lastTimesOnPress[0] = millis();               // last entrance = first index
                this->_lastTimeOnPress = this->_lastTimesOnPress[0]; // store before processing the array
            }
            else
            {
                // ON RELEASE
                this->_isJustReleased = true;
                this->_lastTimeOnRelease = millis();
            }

            this->_btnState = state_; // update current state
        }
    }
    this->_lastState = state_;

    // Check valid clicked
    this->_nValidClicked = this->getNumberOfClicked(this->_lastTimesOnPress);
}

bool Button::isClicked()
{
    return this->_nValidClicked == 1;
}

bool Button::isDoubleClicked()
{
    return this->_nValidClicked == 2;
}

bool Button::isTripleClicked()
{
    return this->_nValidClicked == 3;
}

bool Button::isOnClickRelease()
{
    if (this->isClicked())
    {
        while (this->isPressed())
        {
            this->update();
        }
        return true;
    }
    return false;
}

bool Button::isOnDoubleClickRelease()
{
    if (this->isDoubleClicked())
    {
        while (this->isPressed())
        {
            this->update();
        }
        return true;
    }
    return false;
}

bool Button::isOnTripleClickRelease()
{
    if (this->isTripleClicked())
    {
        while (this->isPressed())
        {
            this->update();
        }
        return true;
    }
    return false;
}

unsigned int Button::timeHold()
{
    if (this->isPressed())
    {
        return millis() - this->_lastTimeOnPress;
    }
    return 0;
}

unsigned int Button::timeHoldOnRelease()
{
    int duration_ = this->_lastTimeOnRelease - this->_lastTimeOnPress;
    return duration_ > 0 ? duration_ : 0;

    return 0;
}

int Button::getNumberOfClicked(unsigned long *timesArray_)
{
    int nClick_ = 0;

    if (timesArray_[0] > this->_delayReClick && (millis() - timesArray_[0] < 10 * this->_delayReClick))
    {
        // 1 - Compute clicks time interval
        for (int i = 0; i < this->_maxReClick - 1; i++)
        {
            if (timesArray_[i] - timesArray_[i + 1] <= this->_delayReClick)
            {
                nClick_++;
            }
            else
            {
                break;
            }
        }

        // 2 - Check last click
        if (nClick_ < this->_maxReClick - 1)
        {
            if (millis() - timesArray_[0] > this->_delayReClick)
            {
                nClick_++; // wait delayReClick before validate if not max N click
            }
            else
            {
                nClick_ = 0; // wait a bit more
            }
        }
        else
        {
            nClick_++; // validate straight forward if max N click (no need to wait for extra click)
        }

        // 3 - Flush collection if click validated
        if (nClick_)
        {
            for (size_t i = 0; i < this->_maxReClick; i++)
            {
                timesArray_[i] = 0;
            }
        }
    }

    return nClick_;
}

#endif // _MOVUINO_BUTTON_