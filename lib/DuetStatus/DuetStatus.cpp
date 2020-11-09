/*
  DuetStatus.h - Library for determining current duet wifi board status.
  Created by Dmitri Farkov, July 29, 2020.
*/

#include "DuetStatus.h"

DuetStatus::DuetStatus(void (*callback)(DuetState, DuetState))
{
  _currentStatus = DuetState::IDLE;
  _previousStatus = DuetState::IDLE;
  _callback = callback;
  _callback(_currentStatus, _previousStatus);
}

void DuetStatus::setStatus(DuetState status) 
{
  if (_currentStatus == status) {
    return;
  }

  _previousStatus = _currentStatus;
  _currentStatus = status;
  _callback(_currentStatus, _previousStatus);
}

void DuetStatus::setStatus(const char* statusLetter)
{
  if (strcmp(statusLetter, "I") == 0) {
    setStatus(DuetState::IDLE);
    return;
  }
  if (strcmp(statusLetter, "F") == 0) {
    setStatus(DuetState::UPDATING);
    return;
  }
	if (strcmp(statusLetter, "O") == 0) {
    setStatus(DuetState::OFF);
    return;
  }
	if (strcmp(statusLetter, "H") == 0) {
    setStatus(DuetState::HALTED);
    return;
  }
	if (strcmp(statusLetter, "D") == 0) {
    setStatus(DuetState::PAUSING);
    return;
  }
	if (strcmp(statusLetter, "S") == 0) {
    setStatus(DuetState::PAUSED);
    return;
  }
	if (strcmp(statusLetter, "R") == 0) {
    setStatus(DuetState::RESUMING);
    return;
  }
	if (strcmp(statusLetter, "P") == 0) {
    setStatus(DuetState::PROCESSING);
    return;
  }
	if (strcmp(statusLetter, "M") == 0) {
    setStatus(DuetState::SIMULATING);
    return;
  }
	if (strcmp(statusLetter, "B") == 0) {
    setStatus(DuetState::BUSY);
    return;
  }
	if (strcmp(statusLetter, "T") == 0) {
    setStatus(DuetState::CHANGINGTOOL);
    return;
  }
	if (strcmp(statusLetter, "I") == 0) {
    setStatus(DuetState::IDLE);
    return;
  }

  setStatus(DuetState::IDLE);
}

String DuetStatus::getStatusAsString(DuetState status) {
  if (status == DuetState::IDLE) {
    return "IDLE";
  }
  if (status == DuetState::UPDATING) {
    return "UPDATING";
  }
	if (status == DuetState::OFF) {
    return "OFF";
  }
	if (status == DuetState::HALTED) {
    return "HALTED";
  }
	if (status == DuetState::PAUSING) {
    return "PAUSING";
  }
	if (status == DuetState::PAUSED) {
    return "PAUSED";
  }
	if (status == DuetState::RESUMING) {
    return "RESUMING";
  }
	if (status == DuetState::PROCESSING) {
    return "PROCESSING";
  }
	if (status == DuetState::SIMULATING) {
    return "SIMULATING";
  }
	if (status == DuetState::BUSY) {
    return "BUSY";
  }
	if (status == DuetState::CHANGINGTOOL) {
    return "CHANGINGTOOL";
  }

  return "IDLE";
}

DuetState DuetStatus::getStatus()
{
  return _currentStatus;
}