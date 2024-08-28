

//       Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_HeaderBase.h"
#include "ACS_CS_Primitive.h"

#include "ACS_CS_Trace.h"


// ACS_CS_PDU
#include "ACS_CS_PDU.h"


ACS_CS_Trace_TDEF(ACS_CS_PDU_TRACE);



// Class ACS_CS_PDU

ACS_CS_PDU::ACS_CS_PDU()
   : pduHeader(0),
     pduPrimitive(0)
{
}

ACS_CS_PDU::ACS_CS_PDU(const ACS_CS_PDU &right)
	: pduHeader(0),
      pduPrimitive(0)
{

   if (right.pduHeader)
      this->pduHeader = right.pduHeader->clone();

   if (right.pduPrimitive)
      this->pduPrimitive = right.pduPrimitive->clone();

}

ACS_CS_PDU::ACS_CS_PDU (ACS_CS_HeaderBase *header, ACS_CS_Primitive *primitive)
   : pduHeader(header),
     pduPrimitive(primitive)
{

   int primitiveLength = 0;
   this->dont_destroy_primitive = false;

   if (this->pduPrimitive)
	   primitiveLength = this->pduPrimitive->getLength();

   if (this->pduHeader)
      this->pduHeader->setTotalLength(this->pduHeader->getHeaderLength() + primitiveLength);

}


ACS_CS_PDU::~ACS_CS_PDU()
{

   if (this->pduHeader)
   {
      delete this->pduHeader;
      this->pduHeader = 0;
   }

   if (!this->dont_destroy_primitive && this->pduPrimitive)
   {
      delete this->pduPrimitive;
      this->pduPrimitive = 0;
   }

}


ACS_CS_PDU & ACS_CS_PDU::operator=(const ACS_CS_PDU &right)
{

   if (this != &right)
   {
      if (this->pduHeader)
      {
         delete this->pduHeader;
         this->pduHeader = 0;
      }

      if (this->pduPrimitive)
      {
         delete this->pduPrimitive;
         this->pduPrimitive = 0;
      }

      if (right.pduHeader)
         this->pduHeader = right.pduHeader->clone();

      if (right.pduPrimitive)
         this->pduPrimitive = right.pduPrimitive->clone();
   }

   return *this;

}



int ACS_CS_PDU::getBuffer (char* buffer, int size) const
{

   // Suppress warning for not freeing buffer,
   // which we shouldn't since it is a value-result argument.
   //lint --e{429}

   if ( (buffer == 0) || (this->pduHeader == 0) || (this->pduPrimitive == 0) ) // Check memory
   {
	   ACS_CS_TRACE((ACS_CS_PDU_TRACE,
		   "(%t) %s, Line : %d \n"
	       "Error: Null pointer \n", __FUNCTION__, __LINE__));

      return -1;
   }

   // Get data from header and primitive objects
   unsigned short headerLength = this->pduHeader->getHeaderLength();
   unsigned short primitiveLength = this->pduPrimitive->getLength();
   unsigned short pduLength = this->getLength();

   // Check if the PDU fits in the buffer
   if ( size < ( headerLength + primitiveLength )
        || ( size < pduLength ) )
   {
	   ACS_CS_TRACE((ACS_CS_PDU_TRACE,
			"(%t) %s, Line : %d \n"
			"Error: Maximum supported PDU size is exceeded. Supported size = %d and PDU size = %d \n",
			__FUNCTION__, __LINE__, size,
			pduLength > headerLength + primitiveLength ? pduLength: headerLength + primitiveLength));

      return -1;
   }

   int error1 = 1, error2 = 1;
   error1 = this->pduHeader->getBuffer(buffer, headerLength);   // Copy header to front of buffer

   if( primitiveLength == 0 )
   {  // this is the case when we have a variable length primitive; true length is known only after serialization
      primitiveLength = size;
   }

   error2 = this->pduPrimitive->getBuffer(buffer + headerLength, primitiveLength); // Copy primitive to back of buffer

   if ( (error1 && error2) )
      return -1;
   else
      return 0;

}

int ACS_CS_PDU::getLength () const
{

   if (this->pduHeader)
      return this->pduHeader->getTotalLength();
   else
      return 0;

}

const ACS_CS_Primitive  * ACS_CS_PDU::getPrimitive () const
{

   return this->pduPrimitive;

}

const ACS_CS_HeaderBase  * ACS_CS_PDU::getHeader () const
{

   return this->pduHeader;

}

void ACS_CS_PDU::setHeader (ACS_CS_HeaderBase *header)
{

   if (header)
   {
      if (pduHeader)
         delete pduHeader;

      pduHeader = header;

      int primitiveLength = 0;

      if (this->pduPrimitive)
    	  primitiveLength = this->pduPrimitive->getLength();

      this->pduHeader->setTotalLength(this->pduHeader->getHeaderLength() + primitiveLength);
   }

}

void ACS_CS_PDU::setPrimitive (ACS_CS_Primitive *primitive)
{

   if (primitive)
   {
      if (pduPrimitive)
         delete pduPrimitive;

      pduPrimitive = primitive;

      int primitiveLength = this->pduPrimitive->getLength();

      if (this->pduHeader)
         this->pduHeader->setTotalLength(this->pduHeader->getHeaderLength() + primitiveLength);
   }

}

ACS_CS_PDU * ACS_CS_PDU::clone () const
{

   // Suppress warning for not freeing pointers.
   // They are returned to the user and deleted elsewhere
   //lint --e{429}

   ACS_CS_HeaderBase * newHeader = 0;
   ACS_CS_Primitive * newPrimitive = 0;

   if (pduHeader)
      newHeader = pduHeader->clone();

   if (pduPrimitive)
      newPrimitive = pduPrimitive->clone();

   ACS_CS_PDU * newPDU = new ACS_CS_PDU(newHeader, newPrimitive);

   return newPDU;

}

// Additional Declarations

