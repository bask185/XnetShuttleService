#include <Arduino.h>
#include "src/macros.h"

#ifndef TRAINS_H
#define TRAINS_H

typedef struct trains 
{
    uint16  address ;
    uint16  breakingFactor ;
    uint16  acceleratingFactor ;
    uint16  maxSpeed ;
    uint16  pauseTime ;    
} Train ;

extern int8   storeTrain( Train *train ) ;
extern int8   getTrain( Train * train, uint16 currentAddress ) ;
extern void   storeCurrentAddress( uint16 address ) ;
extern uint16 getCurrentAddress( ) ;

#endif