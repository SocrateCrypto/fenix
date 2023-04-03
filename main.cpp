/**                 version 0.0.1
            created by Vets Anatoliy
mailto:
web site: http://34.198.74.194/
19.03.2023
*/
#include "Arduino.h"
#include <GyverTimers.h>
#include "Actuator.h"
//***************************переменные*******************************************

int led = 13;
volatile bool Gidrostress = false;                  // фнкцiя "Gidrostress" короткочасно даэ iмпульс на закривання щоб зняти прижимання язичка замка для вiдкривання
int mode = 0;                                       // режим работы платы(start/stop/revers)
volatile unsigned int Tpause = 5000;                // Пауза мiж положенням ворота вiдкритi та закриттям при автоматичному режимi закривання в ms(1000=1sec)
volatile unsigned int Tdif = 1000;                  // рiзниця в стартI мiж двигуном1 та 2(якщо одна створка маэ пластину яка прижимаэ другу то повинна вiдкр. перша)
volatile float Topen1 = 10000;                      // час роботи двигун1 на вiдкривання
volatile float Topen2 = 10000;                      // час роботи двигун2 на вiдкривання
volatile float Tclose1 = 10000;                     // час роботи двигун1 на закриття
volatile float Tclose2 = 10000;                     // час роботи двигун2 на закриття
volatile unsigned int StartAfterLockInAction = 500; // перiод пiсля притягнення електрозамка для початку руху двигуна1 (або двигун1 та двигун2 при параметрi Tdif  =  0)
volatile bool Avtomat = true;                       // автоматичний режим закривання так нi
volatile unsigned int actionLock = 2000;            // час притягнення електрозамка
volatile bool Single = false;                       // режим роботи з одною створкою
volatile bool CloseAfter4sec = false;               // дострокове закривання при пересiканнi пiдряд двох кiл безпеки  IR
volatile int TimeForRevers = 2000;                  // Час перехiдного процесу при реверсi при спрацюваннi пульта ДК
byte status = 0;
int flagSerialMotorStatus1;
int flagSerialMotorStatus2;
//********************************функции******************************************
void obrobka();
void AutoRevers();

//********************************экземпляры****************************************
Actuator motor1(32, Topen1, Tclose1, 36);
Actuator motor2(34, Topen2, Tclose2, 36);
Reledirection reledirection(36);
Lock lock(30);
Pharos lamp(44);
//********************************void setup*****************************************

void setup()
{

   pinMode(13, OUTPUT);
   pinMode(44, OUTPUT);                 // вывод сигнальной лампы
   pinMode(36, OUTPUT);                 // вывод реле управления направлением, общее для 2-х моторов
   attachInterrupt(1, obrobka, RISING); // iнiцiалiзацiя переривання по входу Pin3
   Timer2.setPeriod(1000);              // прерывание по времени установка периодa
   Timer2.enableISR(CHANNEL_A);         // инициация канала прерывания
}
//*******************************void loop********************************************

void loop()
{
   digitalWrite(led, HIGH); // turn the LED on (HIGH is the voltage level)
   delay(500);              // wait for a second
   digitalWrite(led, LOW);  // turn the LED off by making the voltage LOW
   delay(500);              // wait for a second
}
void obrobka()
{
   Serial.begin(9600);
   Timer2.disableISR(CHANNEL_A);
   switch (status)
   {
   case 0: // начальное положение ворот
      lamp.start();
      lamp.setProgramm(1);
      lock.runLock(actionLock);
      reledirection.head();
      motor1.startAfter(StartAfterLockInAction);
      motor2.startAfter(StartAfterLockInAction + Tdif);
      status = 1;
      Serial.print("state");
      Serial.println(status);

      break;
   case 1: // открывание ворот
      motor1.stop();
      motor2.stop();
      lamp.setProgramm(3);
      status = 4;
      Serial.print("motor1. Position  ");
      Serial.println(motor1.getPosition());
      Serial.print("motor2. Position  ");
      Serial.println(motor2.getPosition());
      Serial.print("state");
      Serial.println(status);
      break;
   case 2:
      reledirection.reversAfterMs(200);
      motor1.startAfter(500 + Tdif);
      motor2.startAfter(500);
      lamp.setProgramm(2);
      status = 3;
      Serial.print("state");
      Serial.println(status);

      break;
   case 3: // закрывание ворот
      motor1.stop();
      motor2.stop();
      lamp.setProgramm(4);
      status = 5;
      Serial.print("motor1. Position  ");
      Serial.println(motor1.getPosition());
      Serial.print("motor2. Position  ");
      Serial.println(motor2.getPosition());
      Serial.print("state");
      Serial.println(status);
      break;
   case 4: // пауза позле открытия, после паузы движение закрытие
      reledirection.head();
      reledirection.reversAfterMs(200);
      int dif;
      dif = ((Tclose1 - motor1.getCounterClose()) - (Tclose2 - motor2.getCounterClose()));

      if (dif > static_cast<signed int>(Tdif))
      {
         motor1.startAfter(500);
         motor2.startAfter(500 + abs(dif - Tdif));
      }
      else
      {

         if ((Tclose2 - motor2.getCounterClose()) == 0)
         {

            motor1.startAfter(500);
         }
         else
         {
            motor2.startAfter(500);
            motor1.startAfter(500 + abs(Tdif - dif));
         }
      }
      lamp.setProgramm(2);
      status = 3;
      Serial.print("state");
      Serial.println(status);
      break;
   case 5: // пауза позле закрытия, после паузы движение открытие
      reledirection.back();
      reledirection.reversAfterMs(200);
      motor1.startAfter(500);
      motor2.startAfter(500);
      lamp.setProgramm(1);
      status = 1;
      Serial.print("state");
      Serial.println(status);
      break;

   default:
      status = 0;
      break;
   }
   Serial.end();
   Timer2.enableISR(CHANNEL_A);
}

ISR(TIMER2_A)
{
   Serial.end();
   reledirection.loop();
   motor1.loop();
   motor2.loop();
   lock.loop();
   lamp.loop();
   if (motor1.getState() != flagSerialMotorStatus1)
   {
      Serial.begin(9600);
      Serial.print("motor1   ");
      Serial.println(motor1.dir(motor1.state));
      flagSerialMotorStatus1 = motor1.getState();
   }
   if (motor2.getState() != flagSerialMotorStatus2)
   {
      Serial.begin(9600);
      Serial.print("motor2   ");
      Serial.println(motor2.dir(motor2.state));
      flagSerialMotorStatus2 = motor2.getState();
   }

   if ((motor1.getPosition() == 1000) && (motor2.getPosition() == 1000) && (status != 2) && (motor1.getState() == 0) && (motor2.getState() == 0))
   {
      reledirection.head();
      Serial.begin(9600);
      lamp.setProgramm(3);
      Serial.print("motor1. Position  ");
      Serial.println(motor1.getPosition());
      Serial.print("motor2. Position  ");
      Serial.println(motor2.getPosition());
      status = 2;
      Serial.print("state");
      Serial.println(status);
      if (Avtomat == true)
      {
         AutoRevers();
      }

      Serial.end();
   }
   if ((motor1.getPosition() == 0) && (motor2.getPosition() == 0) && (status != 0) && (motor1.getState() == 0) && (motor2.getState() == 0))
   {
      Serial.begin(9600);
      reledirection.head();
      Serial.print("motor1. Position  ");
      Serial.println(motor1.getPosition());
      Serial.print("motor2. Position  ");
      Serial.println(motor2.getPosition());
      status = 0;
      lamp.stop();

      Serial.print("state");
      Serial.println(status);
      Serial.end();
   }
}
void AutoRevers()
{
   reledirection.reversAfterMs(200);
   motor1.startAfter(500 + Tdif + Tpause);
   motor2.startAfter(500 + Tpause);
   lamp.setProgramm(5);
   lamp.setProgrammAfter(2, Tpause);
   status = 3;
    Serial.print("autorevers after ");
      Serial.println(Tpause);
   Serial.print("state");
   Serial.println(status);
}