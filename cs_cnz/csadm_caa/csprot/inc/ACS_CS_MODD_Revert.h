/*
 * ACS_CS_MODD_Revert.h
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */

#ifndef ACS_CS_MODD_REVERT_H_
#define ACS_CS_MODD_REVERT_H_

#include "ACS_CS_Primitive.h"

#include <string>

#include "ACS_CS_Protocol.h"



class ACS_CS_MODD_Revert: public ACS_CS_Primitive
{

	public:
		ACS_CS_MODD_Revert();

		ACS_CS_MODD_Revert(const ACS_CS_MODD_Revert &right);

		virtual ~ACS_CS_MODD_Revert();

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

		virtual int setBuffer (const char *buffer, int size);

		virtual int getBuffer (char *buffer, int size) const;

		virtual unsigned short getLength () const;

		virtual ACS_CS_Primitive * clone () const;

		int getMacAddress(char * mac, int size) const;

		void setMacAddress(char * mac, int size);

	private:
		ACS_CS_MODD_Revert & operator=(const ACS_CS_MODD_Revert &right);

		char * macAddress;
};


#endif /* ACS_CS_MODD_REVERT_H_ */



