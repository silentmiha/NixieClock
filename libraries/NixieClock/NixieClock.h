#ifndef NixieClock_h
#define NixieClock_h

#include "Arduino.h"

#define NIXIE_VERSION "0.0.1"

enum digitMode{
  
}

class NixieClock {
  public:
    NixieClock(int indicators_count, int spacers_groups, int spacers_count);
    void SetDigitsMode(int mode);
    void Display();
    void initK155ID1Pins(int a0, int a1, int a2, int a3);  
    void intiAnidesPins(int *pins, int count);
    
  private:
    int digits_count; //кол-во задействованных индикаторов
    int spacers_groups // кол-во групп разделителей
    int spacers_count; // кол-во разделителей в группе

    //пины для управления микросхемой К155ИД1
    int id1[4];

    //пины для управления анодами
    int anodes[10];



};


#endif