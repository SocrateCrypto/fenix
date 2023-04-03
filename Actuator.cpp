/**                 version 0.0.1
            created by Vets Anatoliy
mailto:
web site: http://34.198.74.194/
19.03.2023
*/

#include "Arduino.h"
#include "Actuator.h"


Actuator::Actuator(int pin, long Topen, long Tclose, int releDirection)
{
    _pin = pin;
    _Topen = Topen;
    _Tclose = Tclose;
    _releDirection = releDirection;
    counterClose = _Tclose;
    pinMode(_pin, OUTPUT);
    pinMode(_releDirection, OUTPUT);
};
void Actuator::setActuatorInPosition(int setPosition){

};

void Actuator::loop()
{
    counterPositionLoop();
    startAfterLoop();
};

//******************************************************
void Actuator::start()
{

    if ((position >= 0) && (digitalRead(_releDirection) == LOW))
    {
        digitalWrite(_pin, HIGH);
        state = actuatorState::RUN_head;
    }
    if ((position <= 1000) && (digitalRead(_releDirection) == HIGH))
    {
        digitalWrite(_pin, HIGH);
        state = actuatorState::RUN_back;
    }
};
//*********************************************************
void Actuator::stop()
{
    counterTimerActuator=0;
    digitalWrite(_pin, LOW);
  state = actuatorState::OFF;
};
//*********************************************************
void Actuator::startAfter(int ms)
{

    counterTimerActuator = ms;
    state = actuatorState::ON_TIME;
};
//*********************************************************
int Actuator::getPosition()
{

    return position;
};
//*********************************************************
Actuator::actuatorState Actuator::getState()
{
    return Actuator::state;
};
//***********************************************************

void Actuator::counterPositionLoop()
{
    if (digitalRead(_pin) == HIGH)
    {
        if (digitalRead(_releDirection) == HIGH)
        {
            ++counterClose;
            counterClose = constrain(counterClose, 0, _Tclose);
            position = ((1000 * (_Tclose - counterClose)) / _Tclose);
            counterOpen = (position * _Topen) / 1000;
            counterOpen = constrain(counterOpen, 0, _Topen);
            state = actuatorState::RUN_back;

            if (counterClose >= _Tclose)
            {
                digitalWrite(_pin, LOW);
                state = actuatorState::OFF;
            }
        }
        else
        {
            ++counterOpen;
            counterOpen = constrain(counterOpen, 0, _Topen);
            position = ((counterOpen * 1000) / _Topen);
            counterClose = ((1000 - position) * _Tclose) / 1000;
            counterClose = constrain(counterClose, 0, _Tclose);
            state = actuatorState::RUN_head;
            if (counterOpen >= _Topen)
            {
                digitalWrite(_pin, LOW);
                state = actuatorState::OFF;
            }
        }
    }
};
void Actuator::startAfterLoop()
{
    if (counterTimerActuator > 0)
    {
        state = actuatorState::ON_TIME;
        --counterTimerActuator;
        if (counterTimerActuator == 0)
        {
            digitalWrite(_pin, HIGH);
        }
    }
};
long Actuator:: getCounterOpen()
{
return counterOpen;
};
long Actuator:: getCounterClose()
{
return counterClose;
};


//********************************************************************
//********************************************************************
Reledirection::Reledirection(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
};
//*****************************************
void Reledirection::head()
{
    _counter = 0;
    digitalWrite(_pin, LOW);
};
//*****************************************
void Reledirection::back()
{
    _counter = 0;
    digitalWrite(_pin, HIGH);
};
//*****************************************
void Reledirection::revers()
{
    digitalWrite(_pin, (!digitalRead(_pin)));
};
//*****************************************

void Reledirection::reversAfterMs(int ms)
{
    _counter = ms;
};
//*********************************************
void Reledirection::loop()
{
    if (_counter > 0)
    {

        --_counter;
        if (_counter == 0)
        {
            digitalWrite(_pin, (!digitalRead(_pin)));
        }
    }
};
//***********************************************************************************
const void Reledirection::resetCounter()
{
    _counter = 0;
};
//***************************************************************************
//***************************************************************************
Lock::Lock(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
};
//***************************************************************************
void Lock::loop()
{
    if (_duration > 0)
    {

        --_duration;
        if (_duration == 0)
        {
            digitalWrite(_pin, LOW);
        }
    }
};
//*******************************************************************************
void Lock::runLock(int ms)
{
    _duration = ms;
    digitalWrite(_pin, HIGH);
};
//**********************************************************************
//*************************Pharos***************************************
//**********************************************************************
Pharos::Pharos(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    _flag = false;
    _durationOffImpuls = 250;
    _numberOffProgramm = 1;
    ptrArray = _programm1;
    _sizeArray = 8;
};
void Pharos::loop()
{

    if (_flag == true)
    {
        if ((_counter % _durationOffImpuls) == 0)
        {
            digitalWrite(_pin, ptrArray[n]);
            if (n < (_sizeArray - 1))
            {
                ++n;
            }
            else
            {
                n = 0;
                _counter = 0;
            }
        }

        ++_counter;
    }
    if (_counterStartAfter>0)
    {
        -- _counterStartAfter;
        if (_counterStartAfter==0)
        {
           void setProgramm(int _numberProgramm);
            
        }
        
    }
    
};
//***********************************************************************
void Pharos::setArray(bool array[], int sizeArray)
{
    _sizeArray = sizeArray;
    ptrArray = array;
};
void Pharos::setArray(bool array[], int sizeArray, int durationOffImpuls)
{
    _sizeArray = sizeArray;
    ptrArray = array;
    _durationOffImpuls = durationOffImpuls;
};
//**********************************************************************
void Pharos::setDurationOffImpuls(int ms)
{
    _durationOffImpuls = ms;
};
//***********************************************************************
void Pharos::start()
{
    _flag = true;
    _counter = 0;
    n = 0;
};
//***********************************************************************
void Pharos::stop()
{
    _flag = false;
    digitalWrite(_pin, LOW);
    _counter = 0;
    n = 0;
};
//**************************************************************************
void Pharos::setProgramm(int numberOffProgramm)
{
    switch (numberOffProgramm)
    {
    case 1:
        ptrArray = _programm1;
        _sizeArray = 8;
        _durationOffImpuls = 250;
        break;
    case 2:
        ptrArray = _programm2;
        _sizeArray = 4;
        _durationOffImpuls = 250;
        break;
    case 3:
        ptrArray = _programm3;
        _sizeArray = 8;
        _durationOffImpuls = 200;
        break;
    case 4:
        ptrArray = _programm4;
        _sizeArray = 6;
        _durationOffImpuls = 200;
        break;
        case 5:
        ptrArray = _programm5;
        _sizeArray = 6;
        _durationOffImpuls = 200;
        break;
    default:
        ptrArray = _programm1;
        _sizeArray = 8;
        _durationOffImpuls = 250;
        break;
    }
    _counter = 0;
    n = 0;
};
//*****************************************************************************
  void Pharos::setProgrammAfter(int numberOffProgramm,unsigned int time)
  {
    _numberProgramm =numberOffProgramm;
    _counterStartAfter= time;




  };


const char* Actuator:: dir(actuatorState state)
 {
    if(state==0){
        return "OFF";
    } else if(state==1){
        return "run head";
    } else if(state==2){
        return "run back";
    } else if (state==3){
        return "waiting for timer";
    }

return "error";

 }