#include "trains.h"
#include <EEPROM.h>

const int LOCO_ADDRESS_BANK = 1022 ;
const int nTrains = 100 ;

int8 getTrain( Train * train, uint16 currentAddress )
{

    for (int i = 0; i < nTrains; i++ )
    {
        uint16 eeAddress = i * sizeof( *train ) ;
        uint16 locoAddress ;

        EEPROM.get( eeAddress, train ) ;
        if( train->address == currentAddress ) return 1 ;
    }

    return -1 ; // train not found in database
}


int8 storeTrain( Train *train )
{
    for (int i = 0; i < nTrains; i++ )
    {
        Train local ;

        uint16 eeAddress = i * sizeof( local ) ;

        EEPROM.get( eeAddress, local ) ;

        if( local.address == 0xFFFF )                                           // if empty slot found
        {
            EEPROM.put( eeAddress, train ) ;
            return 1 ;
        }
    }

    return -1 ;
}




void storeCurrentAddress( uint16 address )
{
    uint16 eeAddress = LOCO_ADDRESS_BANK ;

    EEPROM.put( eeAddress, address ) ;
}

uint16 getCurrentAddress()
{
    uint16 eeAddress = LOCO_ADDRESS_BANK ;
    uint16 locoAddress ;
    
    EEPROM.get( eeAddress, locoAddress ) ;

    return locoAddress ;
}
