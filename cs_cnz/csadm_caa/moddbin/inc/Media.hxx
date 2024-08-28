//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
//  **************************************************************************
//
//  File Name ........@(#)fn 
//  Document Number ..@(#)dn 
//  Revision Date ....@(#)rd 
//  Current Version ..@(#)cv 
//  Responsible ............ UAB/Y/SG Martin Wahlstrom
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//  The Media class provides a recommended interface for classes intended to
//  be used for communication with other threads / processes.
//>

#ifndef MEDIA_HXX
#define MEDIA_HXX

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "PlatformTypes.hxx"
#include "ObjectStatus.hxx"
#include <cstddef> // Definition of size_t

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup comm
//
//  The Media class forms the top of the Media class hierarchy.
//
//  Class relationships.
//
//  \verbatim
//  +---------
//  |         |
//  |  Media  |
//  |         |
//  +---------
//  \endverbatim
//>
class Media
{
public:
   //< \ingroup commAPI
   //
   //  Enumeration type used by all medias to report their status.
   //>
   enum Status
   {
      OK,
      NotOK,
      Connected, 
      Disconnected, 
      IsFull,
      AddressAlreadyInUse
   };

   // Force the derived classes to implement a destructor.
   virtual ~Media() = 0;

   //< \ingroup commAPI
   //
   // Method for sending of data. Returns MediaOK if the send was successful,
   // other possible return values are MediaNotOK and MediaDisconnected.
   //>
   virtual Status send(u_char*) = 0;

   //< \ingroup commAPI
   //
   // Method for polling of data. The returned size is non 0 if data was
   // found.
   //>
   virtual void poll(u_char*&, size_t&) = 0;

   //< \ingroup commAPI
   //
   // Method used to tell the Media instance that the data returned by the
   // poll method has now been handled.
   //>
   virtual void polledDataIsHandled() = 0;

   //< \ingroup commAPI
   //
   // Method for establishing a connection to the remote Media.
   //>
   virtual Media::Status connect() = 0;

   //< \ingroup commAPI
   //
   // Method for disconnecting an established link with another Media.
   //>
   virtual void disconnect() = 0;

   //< \ingroup commAPI
   //
   // Method to 'reset' the instance to the same state that it was in just
   // after beeing instantiated.
   //>
   virtual void returnToInitialState() = 0;

   //< \ingroup commAPI
   //
   // Returns the current status of the Media instance.
   //>
   virtual ObjectStatus status() const = 0;

protected:
   // The default constructor has an empty implementation. It must exist to
   // support derived class instantiations.
   Media();

private:
   // The copy constructor is declared but not implemented to prohibit the
   // copying of instantiated Medias.
   Media(const Media&);

   // The assignment operator is declared but not defined in order to prevent
   // copying information between Medias.
   Media& operator=(const Media&);

   // Define the send method with a const parameter to generate a compile
   // time error when someone tries to use the method believing that the
   // send member does not change the contents of the parameter.
   Media::Status send(const u_char*);
};

#endif // MEDIA_HXX
