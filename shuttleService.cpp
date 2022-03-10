#include "shuttleService.h"
#include "src/stateMachineClass.h"
#include "event.h"
#include "points.h"
#include "src/io.h"

static StateMachine sm ;

Train train ;

extern uint16 currentAddress ;
uint8 detectorState ;
uint8 speed ; 
uint8 direction;

void initShuttleService()
{
    sm.setState( awaitingTrain ) ;
}

Train getSettings()
{
    if( getTrain( &train, currentAddress ) == -1                                // if either no valid address is entered or teachin button is low -> use potentiometer values
    ||  mode == teachin )                         
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

StateFunction( awaitingTrain )
{
    entryState
    {
        
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

StateFunction( braking )
{
    entryState
    {
        setLights( trainBraking ) ;
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
        setPoints( 0 ) ;                                                        // set street back to main track

        setLights( trainArrived ) ;
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
        if( sm.timeout() && mode == running ) sm.exit() ;                       // if time has passed and train is allowed to depart
    }
    exitState
    {
        setPoints( 1 ) ;                                                        // set the street to the siding before accelerating

        return 1 ;
    }
}

StateFunction( accelerating )
{
    entryState
    {
        setLights( trainDeparting ) ;
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
            setLights( trainNotDeparted ) ;
        }
    }
    exitState
    {
        setLights( trainDeparted ) ;
        return 1 ;
    }
}

uint8_t shuttleService()
{
    STATE_MACHINE_BEGIN(sm)
    {
        State( awaitingTrain )  sm.nextState( braking,         0 ) ;               // wait for sensor to be tripped
        State( braking )        sm.nextState( pausing,      5000 ) ;               // start slowing down
        State( pausing )        sm.nextState( accelerating, 5000 ) ;               // toggle direction after 5s and wait before accelerating, also sets points before accelerating
        State( accelerating )   sm.nextState( departure,       0 ) ;               // accelerate train
        State( departure)       sm.nextState( awaitingTrain,         0 ) ;               // wait for train to leave the sensor, 20 second timeout

    } STATE_MACHINE_END(sm)
}