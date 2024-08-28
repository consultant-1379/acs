// ******************************************************************************
//
//  NAME
//    ACS_ACA_MessageNumber.C
//
//  COPYRIGHT
//    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein 
//    is the property of Ericsson Utvecklings AB, Sweden.
//    The program(s) may be used and/or copied only with 
//    the written permission from Ericsson Utvecklings AB or in 
//    accordance with the terms and conditions stipulated in the 
//    agreement/contract under which the program(s) have been 
//    supplied.

//  DOCUMENT NO
//    CAA 109 0313

//  AUTHOR 
//    UAB/I/GD Carl Johannesson

//  REVISION
//    A
//    2000-05-10

//  DESCRIPTION
//    This class represents a Message Number of a Message Store
//    message. It can be used together with integers in arithmetic
//    operations.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO      DATE        NAME         DESCRIPTION
//    A           2000-05-10  uabcajn      product release
//******************************************************************************

//#include <ACS_ACA_MessageNumber.H>
#include <iostream>
//#include <strstream>
#include <string.h>
#include <ace/ACE.h>
#include <acs_aca_message_number.h>
//using namespace std;

// -----------------------------------------------------------------------------
// Plain constructor. 

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber()
{
    myData.halves[0] = 0;
    myData.halves[1] = 0;
}

// -----------------------------------------------------------------------------
// Constructor taking an array of octets as initial myData.

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber( 
            const unsigned char initAsOctets[ACS_ACA_MessageNumber::OctetSize])
{
    myData.octets[0] = initAsOctets[0]; myData.octets[1] = initAsOctets[1];
    myData.octets[2] = initAsOctets[2]; myData.octets[3] = initAsOctets[3];
    myData.octets[4] = initAsOctets[4]; myData.octets[5] = initAsOctets[5];
    myData.octets[6] = initAsOctets[6]; myData.octets[7] = initAsOctets[7];
}

// -----------------------------------------------------------------------------
// Constructor taking two longs as initial myData.

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber( 
            const unsigned long initAsULongs[ACS_ACA_MessageNumber::LongSize])
{
    myData.halves[0] = initAsULongs[0];
    myData.halves[1] = initAsULongs[1];
}

// -----------------------------------------------------------------------------
// Constructor taking an unsigned integer as initial myData.

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber( const unsigned int initAsUInt )
{
    myData.halves[0] = 0;
    myData.halves[1] = initAsUInt;
}

// -----------------------------------------------------------------------------
// Constructor taking a text string as initial data.

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber( const char* initAsString )
{
    // Is it a hex number and very long?
    //
    if( initAsString[0] == '0' && initAsString[1] == 'x' 
        && strlen( initAsString ) > 10 )
    {
        char    upperHalf[11];
        char    lowerHalf[11];
        
        ACE_OS::memcpy( upperHalf, initAsString, 10 );
        upperHalf[10] = 0;
        
        ACE_OS::memcpy( lowerHalf, "0x", 2 );
        ACE_OS::memcpy( &lowerHalf[2], &initAsString[10], 8 );
        lowerHalf[10] = 0;
        
        myData.halves[0] = strtoul( upperHalf, 0, 16 );
        myData.halves[1] = strtoul( lowerHalf, 0, 16 );
    }
    else
    {
        myData.halves[0] = 0;
        myData.halves[1] = strtoul( initAsString, 0, 0 );
    }
}

// -----------------------------------------------------------------------------
// Copy constructor. 

ACS_ACA_MessageNumber::ACS_ACA_MessageNumber( 
                          const ACS_ACA_MessageNumber& copy)
{
    myData.halves[0] = copy.myData.halves[0];
    myData.halves[1] = copy.myData.halves[1];
}

// -----------------------------------------------------------------------------
// Assignment.

const ACS_ACA_MessageNumber&
        ACS_ACA_MessageNumber::operator=( const ACS_ACA_MessageNumber& number )
{
    // Check assignment to self...
    //
    if( &number != this )
    {
        myData.halves[0] = number.myData.halves[0];
        myData.halves[1] = number.myData.halves[1];
    }

    return *this;
}

// -----------------------------------------------------------------------------
// Get the number as two unsigned long parts.

void
ACS_ACA_MessageNumber::getNumber( unsigned long& upperFourBytes, 
                                  unsigned long& lowerFourBytes ) const
{
    upperFourBytes = myData.halves[0];
    lowerFourBytes = myData.halves[1];
}

// -----------------------------------------------------------------------------
// Get the number copied into supplied buffer.

void
ACS_ACA_MessageNumber::getNumberIntoBuffer( unsigned char 
                                     buffer[ACS_ACA_MessageNumber::OctetSize] )
                                                                           const
{
    for( int i = 0; i < ACS_ACA_MessageNumber::OctetSize; i++ )
    {
        buffer[i] = myData.octets[i];
    }
}

// -----------------------------------------------------------------------------
// Compare for equality.

int
ACS_ACA_MessageNumber::operator==( const ACS_ACA_MessageNumber& n ) const
{
    return (myData.halves[0] == n.myData.halves[0]) && 
           (myData.halves[1] == n.myData.halves[1]);
}

// -----------------------------------------------------------------------------
// Compare for unequality.

int
ACS_ACA_MessageNumber::operator!=( const ACS_ACA_MessageNumber& n ) const
{
    return (myData.halves[0] != n.myData.halves[0]) || 
           (myData.halves[1] != n.myData.halves[1]);
}

// -----------------------------------------------------------------------------
// Compare for less than.

int
ACS_ACA_MessageNumber::operator<( const ACS_ACA_MessageNumber& n ) const
{
    if( myData.halves[0] != n.myData.halves[0] )
    {
        return (myData.halves[0] < n.myData.halves[0]);
    }
    else
    {
        return (myData.halves[1] < n.myData.halves[1]);
    }
}

// -----------------------------------------------------------------------------
// Compare for greater than.

int
ACS_ACA_MessageNumber::operator>( const ACS_ACA_MessageNumber& n ) const
{
    if( myData.halves[0] != n.myData.halves[0] )
    {
        return (myData.halves[0] > n.myData.halves[0]);
    }
    else
    {
        return (myData.halves[1] > n.myData.halves[1]);
    }
}
// -----------------------------------------------------------------------------
// Compare for less or equal than.

int
ACS_ACA_MessageNumber::operator<=( const ACS_ACA_MessageNumber& n ) const
{
    if( myData.halves[0] != n.myData.halves[0] )
    {
        return (myData.halves[0] <= n.myData.halves[0]);
    }
    else
    {
        return (myData.halves[1] <= n.myData.halves[1]);
    }
}

// -----------------------------------------------------------------------------
// Compare for greater or equal than.

int
ACS_ACA_MessageNumber::operator>=( const ACS_ACA_MessageNumber& n ) const
{
    if( myData.halves[0] != n.myData.halves[0] )
    {
        return (myData.halves[0] >= n.myData.halves[0]);
    }
    else
    {
        return (myData.halves[1] >= n.myData.halves[1]);
    }
}

// -----------------------------------------------------------------------------
// Addition operator.

ACS_ACA_MessageNumber
ACS_ACA_MessageNumber::operator+( const ACS_ACA_MessageNumber& n )
{
    ACS_ACA_MessageNumber   tmp(*this);
    
    return tmp+=n;
}

// -----------------------------------------------------------------------------
// Addition assignment operator.

ACS_ACA_MessageNumber&
ACS_ACA_MessageNumber::operator+=( const ACS_ACA_MessageNumber& n )
{
    unsigned long msbBefore = myData.halves[1] & 0x80000000;
    unsigned long msbAfter = 0;
    
    myData.halves[1]+= n.myData.halves[1];
    
    msbAfter = myData.halves[1] & 0x80000000;

    // See if we got overflow.
    //
    if( msbBefore != 0 && msbAfter == 0 )
    {
        myData.halves[0]++;
    }

    myData.halves[0]+= n.myData.halves[0];
    
    return *this;
}

// -----------------------------------------------------------------------------
// Subtraction operator.

ACS_ACA_MessageNumber
ACS_ACA_MessageNumber::operator-( const ACS_ACA_MessageNumber& n )
{
    ACS_ACA_MessageNumber   tmp(*this);

    return tmp-=n;
}

// -----------------------------------------------------------------------------
// Subtraction assignment operator.

ACS_ACA_MessageNumber&
ACS_ACA_MessageNumber::operator-=( const ACS_ACA_MessageNumber& n )
{
    if( n.myData.halves[1] > myData.halves[1] )
    {
        myData.halves[0]--;
    }
    
    myData.halves[1] -= n.myData.halves[1];    
    myData.halves[0] -= n.myData.halves[0];    
    
    return *this;
}

// -----------------------------------------------------------------------------
// Prefix incrementor.

ACS_ACA_MessageNumber& 
ACS_ACA_MessageNumber::operator++()
{
    return operator+=( 1 );
}
// -----------------------------------------------------------------------------
// Postfix incrementor.

ACS_ACA_MessageNumber& 
ACS_ACA_MessageNumber::operator++( int )
{
    return operator+=( 1 );
}

// -----------------------------------------------------------------------------
// Cast to string.

ACS_ACA_MessageNumber::operator const char*() const
{
    const int          bufferSize = 32;
    static char        buffer[bufferSize];
    char               dummy[bufferSize];
//    ostrstream         os( buffer, bufferSize );

    for( int i = 0; i < bufferSize; i++ )
    {
        buffer[i] = 0;
    }
    
    if( myData.halves[0] == 0 )
    {
//        os << myData.halves[1];
		ACE_OS::itoa(myData.halves[1],buffer,10);
    	//ACE_OS::snprintf(myData.halves[1],buffer,10);
    }
    else
    {
//        os << "0x" ;
//        os.width( 8 );
//        os.fill( '0' );
//        os << hex << myData.halves[0];
//        os.width( 8 );
//        os.fill( '0' );
//        os << hex << myData.halves[1] << '\0';
    	ACE_OS::strcpy(buffer,"0x");
    	ACE_OS::strcat(buffer,ACE_OS::itoa(myData.halves[0],dummy,10));
    	ACE_OS::strcat(buffer,ACE_OS::itoa(myData.halves[1],dummy,10));
    }
//    return os.str();
	  return buffer;
}

// -----------------------------------------------------------------------------
// Cast to unsigned long.

ACS_ACA_MessageNumber::operator unsigned long() const
{
    return myData.halves[1];
}

// -----------------------------------------------------------------------------
// Set the number as two unsigned long parts.

void
ACS_ACA_MessageNumber::setNumber( unsigned long upperFourBytes, 
                                  unsigned long lowerFourBytes )
{
    myData.halves[0] = upperFourBytes;
    myData.halves[1] = lowerFourBytes;
}

// -----------------------------------------------------------------------------

ACS_ACA_MessageNumber operator+
                     ( const unsigned int n, const ACS_ACA_MessageNumber& mn )
{
    ACS_ACA_MessageNumber   tmp(mn);
  
    return tmp+= n;
}

// -----------------------------------------------------------------------------

ACS_ACA_MessageNumber operator-
                     ( const unsigned int n, const ACS_ACA_MessageNumber& mn )
{
    ACS_ACA_MessageNumber   tmp(n);

    return tmp-= mn;

}
// -----------------------------------------------------------------------------

//std::ostream& operator<<( std::ostream& os, const ACS_ACA_MessageNumber& n )
//{
//    os << (const char *)n;
//    return os;
//}

// -----------------------------------------------------------------------------

ACS_ACA_MessageNumber::~ACS_ACA_MessageNumber()
{
    // Empty.
}

// -----------------------------------------------------------------------------

ACS_ACA_MessageNumber
ACS_ACA_MessageNumber::operator+( const int i )
{
    ACS_ACA_MessageNumber   tmp1( *this );
    ACS_ACA_MessageNumber   tmp2( (const unsigned int)i );
    return tmp1 + tmp2;
}

// -----------------------------------------------------------------------------

ACS_ACA_MessageNumber
ACS_ACA_MessageNumber::operator-( const int i )
{
    ACS_ACA_MessageNumber   tmp1( *this );
    ACS_ACA_MessageNumber   tmp2( (const unsigned int)i );
    
    return tmp1 - tmp2;
}
