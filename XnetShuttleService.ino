#include "src/io.h"
#include "src/date.h"
#include "src/io.h"
#include "src/version.h"
#include "src/macros.h"
#include "src/debounceClass.h"
#include "src/XpressNetMaster.h"
#include "trains.h"
#include "shuttleService.h"
#include "event.h"
#include "points.h"


/* TODO 

The modes have to be set properly with the button states
the begin situation should examined, what to do with or without train?
XpressNet wrapper functions must be made to set speed and diretion with

*/

XpressNetMasterClass Xnet ;

Debounce teach(     teachinButton ) ;
Debounce newTrain( newTrainButton ) ;
Debounce detector(    detectorPin ) ;
Debounce onOff(          onOffPin ) ;

volatile uint16 currentAddress ;

uint8 teachState ;
uint8 newTrainState ;
extern uint8 detectorState ;
uint8 onOffState ;

bool  waitingNewAddress ;


void debounceInputs()
{
    REPEAT_MS( 50 )
    {
        teach.debounce() ;
        newTrain.debounce() ;
        onOff.debounce() ;
    } END_REPEAT

    REPEAT_MS( 500 )
    {
        detector.debounce() ;
    } END_REPEAT

    teachState    = teach.getState() ;      // can be both pust as toggle, I think toggle would do better
    newTrainState = newTrain.getState() ;   // note, newTrain should be push button
    detectorState = detector.getState() ;
    onOffState    = onOff.getState() ;      // should be toggle Switch
}

void proccesButtons()
{
    if( teachState == RISING )                                                  // if teach button rises, the current potentiometer must be stored
    {
        Train train ;

        train.address            = currentAddress ;
        train.breakingFactor     = analogRead( brakePin ) ;
        train.acceleratingFactor = analogRead( accelPin ) ;
        train.maxSpeed           = analogRead( speedPin ) ;
        train.pauseTime          = analogRead( pausePin ) ;

        if( storeTrain( &train ) == -1 )
        {
            setLights( slotNotFound ) ;
        }
    }
    if( newTrainState == FALLING || teachState == FALLING )
    {
        waitingNewAddress = true ;
        setLights( waiting4address ) ;
    }

    if( detectorState == FALLING ) setLights( detectorMade ) ;
}

void notifyXNetLocoDrive128( uint16_t Address, uint8_t Speed )
{
    if( waitingNewAddress == true )
    {   waitingNewAddress  = false ;
        
        currentAddress = Address ;
        setLights( addressReceived ) ;
        storeCurrentAddress( currentAddress ) ;
    }
}

void movePoint( uint16 raw )
{
    uint8  state   = raw >> 15 ;
    uint16 address = raw & 0x3FF ;

    Xnet.SetTrntPos( address, state, 1 ) ;
    delay( 20 ) ;
    Xnet.SetTrntPos( address, state, 0 ) ;
}

void setup()
{
    initIO() ;
    initShuttleService() ;
    Xnet.setup( Loco128, RS485dir ) ;

    debounceInputs() ;
    delay( 100 ) ;
    debounceInputs() ;
    delay( 100 ) ;
    debounceInputs() ;
}

void loop()
{
    debounceInputs() ;
    proccesButtons() ;
    handlePoints() ;

    if( onOffState == HIGH ) shuttleService() ;                                  // run state machine

    Xnet.update() ;
}
