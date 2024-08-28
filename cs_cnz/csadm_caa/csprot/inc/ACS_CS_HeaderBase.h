

//	-
//	Copyright Ericsson AB 2007. All rights reserved.
//	-


#ifndef ACS_CS_HeaderBase_h
#define ACS_CS_HeaderBase_h 1



#include "ACS_CS_Protocol.h"









class ACS_CS_HeaderBase
{




	public:
		ACS_CS_HeaderBase();

		virtual ~ACS_CS_HeaderBase();

		virtual ACS_CS_Protocol::CS_Protocol_Type getProtocol() const = 0;

		virtual ACS_CS_Protocol::CS_Version_Identifier getVersion () const = 0;

		virtual unsigned short getHeaderLength () const = 0;

		virtual ACS_CS_Protocol::CS_Scope_Identifier getScope () const = 0;

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveId () const = 0;

		virtual void setVersion (ACS_CS_Protocol::CS_Version_Identifier identifier) = 0;

		virtual void setPrimitive (ACS_CS_Protocol::CS_Primitive_Identifier identifier) = 0;

		virtual int getBuffer (char *buffer, int size) const = 0;

		virtual int setBuffer (const char *buffer, int size) = 0;

		virtual unsigned short getTotalLength () const = 0;

		virtual void setTotalLength (int length) = 0;

		virtual ACS_CS_HeaderBase * clone () const = 0;

	private:
		ACS_CS_HeaderBase & operator=(const ACS_CS_HeaderBase &right);

		ACS_CS_HeaderBase(const ACS_CS_HeaderBase &right);

};


// Class ACS_CS_HeaderBase



#endif
