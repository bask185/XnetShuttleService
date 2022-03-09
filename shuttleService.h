#include <Arduino.h>
#include "trains.h"
#include "src/macros.h"

extern Train train ;

enum states
{
    running,                // green
    breaking,               // green + red flash
    pausing,                // green flashing
    accelerating,           // green + yellow flash
    departure,
} ;

extern void initShuttleService() ;
extern uint8 shuttleService() ;