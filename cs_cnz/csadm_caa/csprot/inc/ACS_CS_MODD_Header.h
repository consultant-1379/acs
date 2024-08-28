/*
 * ACS_CS_MODD_Header.h
 *
 *  Created on: Jan 21, 2011
 *      Author: mann
 */

#ifndef ACS_CS_MODD_HEADER_H_
#define ACS_CS_MODD_HEADER_H_

#include "ACS_CS_HeaderBase.h"

#include "ACS_CS_Protocol.h"




class ACS_CS_MODD_Header: public ACS_CS_HeaderBase
{

	struct MODD_HeaderBuffer
	{
		unsigned short version;
		unsigned short length;
		unsigned short primitive;
		unsigned short reserved;
	};


	public:
		ACS_CS_MODD_Header();

		ACS_CS_MODD_Header(const ACS_CS_MODD_Header &right);

		virtual ~ACS_CS_MODD_Header();

		virtual ACS_CS_Protocol::CS_Protocol_Type getProtocol() const;

		virtual ACS_CS_Protocol::CS_Version_Identifier getVersion () const;

		virtual unsigned short getHeaderLength () const;

		virtual ACS_CS_Protocol::CS_Scope_Identifier getScope () const;

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveId () const;

		virtual void setVersion (ACS_CS_Protocol::CS_Version_Identifier identifier);

		virtual void setPrimitive (ACS_CS_Protocol::CS_Primitive_Identifier identifier);

		virtual int getBuffer (char *buffer, int size) const;

		virtual int setBuffer (const char *buffer, int size);

		virtual unsigned short getTotalLength () const;

		virtual void setTotalLength (int length);

		virtual ACS_CS_HeaderBase * clone () const;

	private:
		ACS_CS_MODD_Header & operator=(const ACS_CS_MODD_Header &right);

		ACS_CS_Protocol::CS_Version_Identifier version;

		short unsigned totalLength;

		ACS_CS_Protocol::CS_Primitive_Identifier primtiveId;


};


// Class ACS_CS_MODD_Header



#endif /* ACS_CS_MODD_HEADER_H_ */
