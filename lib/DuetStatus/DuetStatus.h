/*
  DuetStatus.h - Library for determining current duet wifi board status.
  Created by Dmitri Farkov, July 29, 2020.
*/
#ifndef DuetState_h
#define DuetState_h

#include "Arduino.h"

enum class DuetState {
  OFF,
  HALTED,
  PAUSING,
  PAUSED,
  RESUMING,
  UPDATING,
  PROCESSING,
  SIMULATING,
  BUSY,
  CHANGINGTOOL,
  IDLE,
};

class DuetStatus
{
  public:
    DuetStatus(void (*callback)(DuetState, DuetState));
    void setStatus(DuetState status);
    void setStatus(const char* statusLetter);
    DuetState getStatus();
    static String getStatusAsString(DuetState status);
  private:
    DuetState _currentStatus;
    DuetState _previousStatus;
    void (*_callback)(DuetState, DuetState);
};

#endif
