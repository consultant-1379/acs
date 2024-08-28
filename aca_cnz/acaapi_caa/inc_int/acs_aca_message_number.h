/*=================================================================== */
/**
   @file  acs_aca_message_number.h  

   @brief Header file for aca module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_ACA_MessageNumber_H
#define ACS_ACA_MessageNumber_H


/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
//class ACS_ACA_InternalMessageStoreAccessPoint;
class  ACS_ACA_MessageNumber {
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
    enum DataSize { LongSize = 2, OctetSize = 8 };  // Faking class constants.
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

 */
/*=================================================================== */
    ACS_ACA_MessageNumber();
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

   @param  copy

 */
/*=================================================================== */
    ACS_ACA_MessageNumber( const ACS_ACA_MessageNumber& copy );
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
    virtual ~ACS_ACA_MessageNumber();
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

   @param  initAsOctets

 */
/*=================================================================== */
    ACS_ACA_MessageNumber( const unsigned char initAsOctets[OctetSize] );
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

   @param  initAsULongs

 */
/*=================================================================== */
    ACS_ACA_MessageNumber( const unsigned long initAsULongs[LongSize] );
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

   @param  initAsUInt

 */
/*=================================================================== */
    ACS_ACA_MessageNumber( const unsigned int  initAsUInt );
/*=================================================================== */
/**
   @brief  Constructor for ACS_ACA_MessageNumber class.

   @param  initAsString

 */
/*=================================================================== */
    ACS_ACA_MessageNumber( const char*         initAsString );

    const ACS_ACA_MessageNumber&
        operator=( const ACS_ACA_MessageNumber& number );
/*=================================================================== */
/**
   @brief  getNumber
            Get message number

   @param  upperFourBytes

   @param  lowerFourBytes

   @return void
 */
/*=================================================================== */
    void getNumber( unsigned long& upperFourBytes,
                  unsigned long& lowerFourBytes )const;
/*=================================================================== */
/**
   @brief  getNumberIntoBuffer
            Load octet data into buffer

   @param  buffer

   @return void
 */
/*=================================================================== */
  void getNumberIntoBuffer( unsigned char buffer[OctetSize] ) const;
/*=====================================================================
                          OPERATOR DECLARATION SECTION
==================================================================== */
    int operator==( const ACS_ACA_MessageNumber& n ) const;
    int operator!=( const ACS_ACA_MessageNumber& n ) const;
    int operator<( const ACS_ACA_MessageNumber& n ) const;
    int operator>( const ACS_ACA_MessageNumber& n ) const;
    int operator<=( const ACS_ACA_MessageNumber& n ) const;
    int operator>=( const ACS_ACA_MessageNumber& n ) const;

    ACS_ACA_MessageNumber operator+( const ACS_ACA_MessageNumber& n );
    friend ACS_ACA_MessageNumber operator+
                     ( const unsigned int n, const ACS_ACA_MessageNumber& mn );
    ACS_ACA_MessageNumber operator+( const int n );
                     
    ACS_ACA_MessageNumber& operator+=( const ACS_ACA_MessageNumber& n );

    ACS_ACA_MessageNumber operator-( const ACS_ACA_MessageNumber& n );
    friend ACS_ACA_MessageNumber operator-
                     ( const unsigned int n, const ACS_ACA_MessageNumber& mn );
    ACS_ACA_MessageNumber operator-( const int n );

    ACS_ACA_MessageNumber& operator-=( const ACS_ACA_MessageNumber& n );

    ACS_ACA_MessageNumber& operator++(int); // Postfix.
    ACS_ACA_MessageNumber& operator++();    // Prefix.

    // Conversion to a string.
    // Note that the returned pointer is only valid until next call.
    //
    operator const char* () const;
    
    // Conversion to unsigned long.
    //
    operator unsigned long () const;


//    friend std::ostream& operator<<( std::ostream& os, const ACS_ACA_MessageNumber& n );
/*=====================================================================
                          PROTECTED DECLARATION SECTION
==================================================================== */
protected:
/*=================================================================== */
/**
   @brief  setNumber

   @param  upperFourBytes

   @param  lowerFourBytes

   @return void  
 */
/*=================================================================== */

    void setNumber( unsigned long upperFourBytes, 
                    unsigned long lowerFourBytes );
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                          UNION DECLARATION SECTION
==================================================================== */
    union
    {
        unsigned char octets[OctetSize];
        unsigned long halves[LongSize];
    } myData;
/*=====================================================================
                          FRIEND CLASS DECLARATION SECTION
==================================================================== */
    friend class ACS_ACA_InternalMessageStoreAccessPoint;

};
#endif
