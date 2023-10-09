#if !defined(_BATTERY_MANAGER_)
#define _BATTERY_MANAGER_

#include <Arduino.h>
#include "globals.h"

class BatteryManager
{
  private:
    
public:
  BatteryManager();
  ~BatteryManager();
  void begin();
  int getLevel();
};

BatteryManager::BatteryManager(){ }
BatteryManager::~BatteryManager() {}

void BatteryManager::begin()
{
  pinMode(PIN_BATTERY, INPUT);
}

int BatteryManager::getLevel()
{
  int sum_;
  int level_;

  sum_ = 0;
  for (uint8_t i = 0; i < 10; i++) // Do the average over 10 values
    sum_ += analogRead(PIN_BATTERY);

  if (sum_ < BATTERY_MIN_VAL * 10)
    sum_ = BATTERY_MIN_VAL * 10;

  if (sum_ > BATTERY_MAX_VAL * 10)
    sum_ = BATTERY_MAX_VAL * 10;

  level_ = ((float)((sum_ / 10) - BATTERY_MIN_VAL) / (float)(BATTERY_MAX_VAL - BATTERY_MIN_VAL)) * 100.0;

  return level_;
}

#endif // _BATTERY_MANAGER_
