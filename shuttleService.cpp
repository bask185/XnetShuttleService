#include "shuttleService.h"
#include "src/stateMachineClass.h"
#include "src/io.h"

static StateMachine sm ;

Train train ;

extern uint16 currentAddress ;
uint8 detectorState ;
uint8 speed ; 
uint8 direction;

void initShuttleService()
{
    sm.setState( running ) ;
}


Train getSettings()
{
    if( getTrain( &train, currentAddress ) == -1                                // if either no valid address is entered or teachin button is low -> use potentiometer values
    ||  digitalRead( teachinButton ) == LOW )                           
    {
        train.breakingFactor     = analogRead( brakePin ) ;
        train.acceleratingFactor = analogRead( accelPin ) ;
        train.maxSpeed           = analogRead( speedPin ) ;
        train.pauseTime          = analogRead( pausePin ) ;
    }

    return train ;
}

#define entryState  if( sm.entryState() )
#define onState     if( sm.onState() )
#define exitState   if( sm.exitState() )

StateFunction( running )
{
    entryState
    {
        getSettings() ;
    }
    onState
    {       
        if( detectorState == FALLING ) sm.exit() ;
    }
    exitState
    {
        getSettings() ;

        return 1 ;
    }
}

StateFunction( breaking )
{
    entryState
    {
        
    }
    onState
    {
        if( sm.repeat( train.breakingFactor ) )
        {
            if( speed >= 0 ) speed -- ;
            // Xnet.setLocoDrive( loco128, speed | direction ) ;
        }

        if( speed == 0 ) sm.exit() ;
    }
    exitState
    {
        
        return 1 ;
    }
}

StateFunction( pausing )
{
    entryState
    {
        direction ^= 0x80 ;
        sm.setTimeout( 100 * train.pauseTime ) ;
    }
    onState
    {
        if( sm.timeout() ) sm.exit() ;
    }
    exitState
    {
        
        return 1 ;
    }
}

StateFunction( accelerating )
{
    entryState
    {
        
    }
    onState
    {
        if( sm.repeat( train.breakingFactor ) )
        {
            if( speed <= train.maxSpeed ) speed ++ ;
            // Xnet.setLocoDrive( loco128, speed | direction ) ; // TODO, FLIP SPEED DIRECTION!!
        }
        
        if( speed == train.maxSpeed ) sm.exit() ;
    }
    exitState
    {
        
        return 1 ;
    }
}

StateFunction( departure )
{
    entryState
    {
        sm.setTimeout( 20000 ) ;                                                // train must be free of the section within 20 seconds
    }
    onState
    {
        if( detectorState == RISING ) sm.exit() ;

        if( sm.timeout() )
        {
            // Xnet.setPower( /* kill power */ ) ;
        }
    }
    exitState
    {
        
        return 1 ;
    }
}

uint8_t shuttleService()
{
    STATE_MACHINE_BEGIN(sm)
    {
        State( running )        sm.nextState( breaking,     0 ) ;               // wait for sensor to be tripped
        State( breaking )       sm.nextState( pausing,   5000 ) ;               // start slowing down
        State( pausing )        sm.nextState( accelerating, 0 ) ;               // toggle direction after 5s and wait before accelerating
        State( accelerating )   sm.nextState( departure,    0 ) ;               // accelerate train
        State( departure)       sm.nextState( running,      0 ) ;               // wait for train to leave the sensor, 20 second timeout

    } STATE_MACHINE_END(sm)
}