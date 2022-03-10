#include <Arduino.h>
#include "src/macros.h"

const int FAST = 250 ;
const int SLOW = 500 ;

enum events
{
    runModeEnabled = 1,
    teachingEnabled,
    turnedOff,
    detectorMade,           
    trainBraking,       
    trainArrived,
    trainDeparting,     
    trainNotDeparted, 
    trainDeparted,      
    slotAdded,
    slotNotFound, 
    slotLoaded,
    waiting4address,
    addressReceived,
} ;

enum modes
{
    OFF,
    running,
    teachin
} ;

extern void eventHandler() ;
extern void setLights( uint8 ) ;
extern uint8 mode ;
