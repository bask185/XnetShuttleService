#include "points.h"

uint16 streetIndex ;
uint16  pointIndex ;
uint16  pointAddress ;

extern void movePoint( uint16 ) ;


const int pointInterval = 500 ;

const int C = 0x8000 ;  // curved
const int S = 0x0000 ;  // straight

const int nStreets = 2 ;
const int nPointsPerStreet = 10 ;

#define MODULE_1
// #define MODULE_2
// #define MODULE_3
// #define MODULE_4

#ifdef MODULE_1
const int pointStreet[nStreets][nPointsPerStreet] = 
 { /*************** MODULE 1 **************/
 /* #1 */   { 1|S, 2|C, 0 },        // end with 0 
 /* #2 */   { 1|C, 2|S, 0 },
 } ;
 #elif defined MODULE_2
 const int pointStreet[nStreets][] = 
 {  /*************** MODULE 2 **************/
 /* #1 */   { 1|S, 2|C, 0 },        // end with 0
 /* #2 */   { 1|C, 2|S, 0 },
 } ;
 #elif defined MODULE_3
 const int pointStreet[nStreets][] = 
 {  /*************** MODULE 3 **************/
 /* #1 */   { 1|S, 2|C, 0 },        // end with 0
 /* #2 */   { 1|C, 2|S, 0 },
 } ;
 #elif defined MODULE_4
 const int pointStreet[nStreets][] = 
 {  /*************** MODULE 4 **************/
 /* #1 */   { 1|S, 2|C, 0 },        // end with 0
 /* #2 */   { 1|C, 2|S, 0 },
 } ;
 #endif

void handlePoints()
{
    if( pointAddress == 0 ) return ;

    REPEAT_MS( pointInterval )
    {
        pointAddress = pointStreet[ streetIndex ][ pointIndex++ ] ;
        if( pointAddress == 0 ) return ;

        movePoint( pointAddress ) ;                                             // send the Xnet command

    } END_REPEAT 
}

void setPoints( uint8 _index )
{
    streetIndex = _index ;
    pointIndex  = 0 ;

    pointAddress = pointStreet[streetIndex][ pointIndex++ ] ;                        // already load the first address
}