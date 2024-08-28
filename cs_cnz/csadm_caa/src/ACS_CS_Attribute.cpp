//	Copyright Ericsson AB 2007. All rights reserved.
#include "ACS_CS_Util.h"

#include <sstream>


// ACS_CS_Attribute
#include "ACS_CS_Attribute.h"
#include <cstdlib>
#include <cstring>
#include <limits.h>
#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_Attribute_TRACE);

using std::istream;
using std::ostream;



// Class ACS_CS_Attribute 

ACS_CS_Attribute::ACS_CS_Attribute()
: attributeId(ACS_CS_Protocol::Attribute_NotSpecified),
   value(0),
   length(0),
   good(true)
{
}

ACS_CS_Attribute::ACS_CS_Attribute(const ACS_CS_Attribute &right)
{

   // Assign values
   this->attributeId = right.attributeId;
   this->length = right.length;
   this->good = right.good;
   this->value = 0;

   if (right.value)							// If there is a value to copy
   {
      if (this->length > 0)
         this->value = new char[this->length];	// Allocate memory

      if (this->value)	// If new is successful
         memcpy(this->value, right.value, this->length);	// and copy the value
   }

}

ACS_CS_Attribute::ACS_CS_Attribute (ACS_CS_Protocol::CS_Attribute_Identifier identifier)
: attributeId(ACS_CS_Protocol::Attribute_NotSpecified),
   value(0),
   length(0),
   good(true)
{

   attributeId = identifier;

}


ACS_CS_Attribute::~ACS_CS_Attribute()
{

   if (this->value)
      delete [] this->value;

}


ACS_CS_Attribute & ACS_CS_Attribute::operator=(const ACS_CS_Attribute &right)
{

   if (this != &right)	// Check for assignment to self
   {
      // Assign values
      this->attributeId = right.attributeId;
      this->length = right.length;
      this->good = right.good;

      if (this->value)	// Delete old value
      {
         delete [] this->value;
         this->value = 0;
      }

      if (right.value)	// Copy new value
      {
         if (this->length > 0)
            this->value = new char[this->length];

         if (this->value)	// Check for successful new
            memcpy(this->value, right.value, this->length);
      }
   }

   return *this;

}


int ACS_CS_Attribute::operator==(const ACS_CS_Attribute &right) const
{

   int match = 0;

   if (this->attributeId == right.attributeId)		// Compare attribute id
   {
      if (this->length == right.length)			// Compare value length
      {
         if (this->length > 0)					// Is there a value (may be null)
         {
            if (this->value && right.value)
               if (memcmp(this->value, right.value, this->length) == 0) // Compare memory
                  match = 1;
         }
      }
   }

   return match;

}

int ACS_CS_Attribute::operator!=(const ACS_CS_Attribute &right) const
{

   int match = 1;

   if (this->attributeId == right.attributeId)		// Compare attribute id
   {
      if (this->length == right.length)			// Compare value length
      {
         if (this->length > 0)					// Is there a value (may be null)
         {
            if (this->value != 0)
               if (memcmp(this->value, right.value, this->length) == 0) // Compare memory
                  match = 0;
         }
      }
   }

   return match;

}


int ACS_CS_Attribute::operator<(const ACS_CS_Attribute &right) const
{

   return ( this->attributeId < right.attributeId ? 1 : 0 );	// Compare attribute id

}

int ACS_CS_Attribute::operator>(const ACS_CS_Attribute &right) const
{

   return ( this->attributeId > right.attributeId ? 1 : 0 );	// Compare attribute id

}

int ACS_CS_Attribute::operator<=(const ACS_CS_Attribute &right) const
{

   return ( this->attributeId <= right.attributeId ? 1 : 0 );	// Compare attribute id

}

int ACS_CS_Attribute::operator>=(const ACS_CS_Attribute &right) const
{

   return ( this->attributeId >= right.attributeId ? 1 : 0 );	// Compare attribute id

}


ostream & operator<<(ostream &stream,const ACS_CS_Attribute &right)
{

   // Check if there is data to save
   if ( (right.length > 0) && (right.value != 0) )
   {
      ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
        "(%t) ACS_CS_Attribute::operator<<\n"
        "Writing attribute %d length %d\n", right.attributeId, right.length));

      // Save all internal variables
      unsigned short attId = right.attributeId;
      (void) stream.write( reinterpret_cast<const char *> (&attId), sizeof(attId));
      (void) stream.write( reinterpret_cast<const char *> (&right.length), sizeof(right.length));
      (void) stream.write( reinterpret_cast<const char *> (right.value), right.length);
   }

   return stream;

}

istream & operator>>(istream &stream,ACS_CS_Attribute &object)
{

   unsigned short attId = 0;
   int attLength = 0;
   char * attValue = 0;

   // Read attribute id from stream
   (void) stream.read( (char *) (&attId), sizeof(attId) );

   ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
            "(%t) ACS_CS_Attribute::operator>>\n"
            "Attribute Id read %u\n", attId));

   // Check for valid attribute id
   if (ACS_CS_Protocol::CS_ProtocolChecker::checkAttribute(attId) ||
         ( attId == ACS_CS_Protocol::Attribute_MAC_EthA ) ||
         ( attId == ACS_CS_Protocol::Attribute_MAC_EthB ) )
   {
      ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::operator>>\n"
               "Attribute Id %u is VALID\n", attId));
      object.attributeId = static_cast<ACS_CS_Protocol::CS_Attribute_Identifier> (attId);
   }
   else
   {
       ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::operator>>\n"
               "Error: Attribute Id %u is NOT VALID!!\n", attId));
      object.good = false;
      return stream;
   }

   // Read attribute length from stream
   (void) stream.read( (char *) (&attLength), sizeof(attLength) );
   ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
            "(%t) ACS_CS_Attribute::operator>>\n"
            "Attribute Length %d\n", attLength));

   if ( (attLength > 0) && (attLength < USHRT_MAX) )
   {
      object.length = attLength;
   }
   else
   {
       ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::operator>>\n"
               "Error: Invalid Attribute Length %d\n", attLength));
      object.good = false;
      return stream;
   }

   if (object.length > 0)
   {
      // Create buffer for value
      attValue = new char[object.length];

      // Read value from stream
      if (attValue)
         (void) stream.read( (char *) (attValue), attLength );

      // If stream successfully read
      if (stream.gcount() != object.length )
      {
        ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
                  "(%t) ACS_CS_Attribute::operator>>\n"
                  "Error: stream not successfully read for attrId %d\n", attId));
         object.good = false;
         delete [] attValue;
         return stream;
      }
   }

   // If there is an old value, delete it
   if (object.value)
   {
      delete [] object.value;
      object.value = 0;
   }

   // Save new value
   object.value = attValue;
   object.good = true;

   ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
     "(%t) ACS_CS_Attribute::operator>>\n"
     "Attribute read %d length %d\n", object.attributeId, object.length));

   return stream;

}

int ACS_CS_Attribute::getValue (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
   {
      ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
        "(%t) ACS_CS_Attribute::getValue()\n"
        "Error: No valid pointer\n"));

      return -1;
   }
   else if (this->length > size)	// Check for space
   {
      ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::getValue()\n"
               "Error: Invalid size, size = %d this->length = %d\n",
               size,
               this->length));

      return -1;
   }
   else if (this->value == 0)		// Check if there is data to copy
   {
     ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::getValue()\n"
               "Error: No value to copy\n"));

      return -1;
   }

   memcpy(buffer, this->value, this->length);	// Copy value

   return this->length;

}

int ACS_CS_Attribute::setValue (const char *buffer, int size)
{

   if (buffer == 0)			// Check for valid memory
   {
     ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::setValue()\n"
               "Error: No valid pointer\n"));

      return -1;
   }
   else if ( (size > USHRT_MAX ) || (size < 1) )// Check for valid size
   {
      ACS_CS_TRACE((ACS_CS_Attribute_TRACE,
               "(%t) ACS_CS_Attribute::setValue()\n"
               "Error: Invalid size, size = %d\n",
               size));

      return -1;
   }

   if (this->value)			// Delete old value
      delete [] this->value;

   this->length = size;
   this->value = new char[size];	// Allocate memory for value

   if (this->value)				// Check for successful new
   {
      memcpy(this->value, buffer, this->length);	// Copy value
      return 0;
   }
   else
      return -1;

}

ACS_CS_Protocol::CS_Attribute_Identifier ACS_CS_Attribute::getIdentifier () const
{

   return attributeId;

}

int ACS_CS_Attribute::getValueLength () const
{

   return length;

}

bool ACS_CS_Attribute::isGood () const
{

   return good;

}

// Additional Declarations

