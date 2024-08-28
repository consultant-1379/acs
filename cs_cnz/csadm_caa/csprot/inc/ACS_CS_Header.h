

//	-
//	Copyright Ericsson AB 2007. All rights reserved.
//	-


#ifndef ACS_CS_Header_h
#define ACS_CS_Header_h 1

#include "ACS_CS_HeaderBase.h"

#include "ACS_CS_Protocol.h"









class ACS_CS_Header: public ACS_CS_HeaderBase
{

	struct CS_HeaderBuffer
	{
		unsigned short version;
		unsigned short length;
		unsigned short scope;
		unsigned short primitive;
		unsigned short requestId;
		unsigned short reserved;
	};


	public:
		ACS_CS_Header();

		ACS_CS_Header(const ACS_CS_Header &right);

		virtual ~ACS_CS_Header();

		virtual ACS_CS_Protocol::CS_Protocol_Type getProtocol() const;

		virtual ACS_CS_Protocol::CS_Version_Identifier getVersion () const;

		virtual unsigned short getHeaderLength () const;

		virtual ACS_CS_Protocol::CS_Scope_Identifier getScope () const;

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveId () const;

		virtual unsigned short getRequestId () const;

		virtual void setVersion (ACS_CS_Protocol::CS_Version_Identifier identifier);

		virtual void setScope (ACS_CS_Protocol::CS_Scope_Identifier identifier);

		virtual void setPrimitive (ACS_CS_Protocol::CS_Primitive_Identifier identifier);

		virtual void setRequestId (unsigned short identifier);

		virtual int getBuffer (char *buffer, int size) const;

		virtual int setBuffer (const char *buffer, int size);

		virtual unsigned short getTotalLength () const;

		virtual void setTotalLength (int length);

		virtual ACS_CS_HeaderBase * clone () const;


	private:
		ACS_CS_Header & operator=(const ACS_CS_Header &right);

		ACS_CS_Protocol::CS_Version_Identifier version;

		short unsigned totalLength;

		ACS_CS_Protocol::CS_Scope_Identifier scope;

		ACS_CS_Protocol::CS_Primitive_Identifier primtiveId;

		short unsigned requestId;

};


// Class ACS_CS_Header 



#endif
