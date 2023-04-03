/**                 version 0.0.1
            created by Vets Anatoliy
mailto:
web site: http://34.198.74.194/
19.03.2023
*/

#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"

class Actuator
{

public:
    Actuator(int pin, long Topen, long Tclose, int releDirection);
    enum actuatorState
    {
        OFF,
        RUN_head,
        RUN_back,
        ON_TIME,

    };

    void loop();  /*Данный метод должен крутиться в цикле ISR{} прерывания*/
    void start(); /*Запускает актуатор */
    void stop();  /*Останавливает актуатор */

    void startAfter(int ms); // запускает актуатор после времени ms
                             // void lockStress();

    /*setters-getters*/
    int getPosition();
    long getCounterOpen();
    long getCounterClose();

    void setActuatorInPosition(int setPosition);
    actuatorState getState();
    actuatorState state = OFF;
    const char *dir(actuatorState state);
    //***********************************************************************

private:
    int _pin;
    long _Topen;
    long _Tclose;
    int _releDirection;
    volatile long counterOpen = 0;
    volatile long counterClose;
    long position = 0;
    void counterPositionLoop();
    int counterTimerActuator = 0;
    void startAfterLoop();

    //*****************************************************************
    //*****************************************************************
};
class Reledirection
{
public:
    Reledirection(int pin);
    void head(); // реле в положении close
    void back(); // реле в положении open
    void revers();
    void reversAfterMs(int ms);
    const void resetCounter();
    void loop(); // эта часть должна крутится в цикле ISR прерывания

private:
    int _pin;
    int _counter;
};
//*********************************************************************************
//*********************************************************************************

class Lock
{
public:
    Lock(int pin);
    void loop();
    void runLock(int ms);

private:
    int _duration = 0;
    int _pin;
};
//**********************************************************************************
//**********************************************************************************
class Pharos
{
private:
    bool *ptrArray;
    unsigned int _sizeArray;
    int _pin;
    int _numberOffProgramm = 1;
    int _counter = 0;
    unsigned int n = 0;
    int _durationOffImpuls;
    unsigned int _counterStartAfter;
    bool _flag;
    bool _programm1[8] = {1, 1, 1, 1, 0, 0, 0, 0};
    bool _programm2[4] = {1, 1, 0, 0};
    bool _programm3[8] = {1, 1, 1, 1, 1, 0, 1, 0};
    bool _programm4[6] = {1, 1, 1, 1, 1, 0};
    bool _programm5[6] = {0, 1, 0, 1, 0, 1};
    int _numberProgramm;
   

public:
    Pharos(int pin);
    void loop(); // эта часть должна крутится в цикле ISR прерывания
    void start();
    void setProgrammAfter(int numberOffProgramm,unsigned int time);
    void stop();
    /*setters-getters*/
    void setProgramm(int numberOffProgramm);
    void setArray(bool array[], int sizeArray);
    void setArray(bool array[], int sizeArray, int durationOffImpuls);
    void setDurationOffImpuls(int ms);
};
//*******************************************************************************
//*******************************************************************************

#endif
