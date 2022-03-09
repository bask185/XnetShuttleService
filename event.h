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
    trainBreaking,       
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

extern void eventHandler() ;
extern void setLights( uint8 ) ;