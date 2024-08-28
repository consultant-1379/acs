/*
 * ACS_CS_MODD_Change.h
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */

#ifndef ACS_CS_MODD_CHANGE_H_
#define ACS_CS_MODD_CHANGE_H_

#include "ACS_CS_Primitive.h"

#include <string>

#include "ACS_CS_Protocol.h"



class ACS_CS_MODD_Change: public ACS_CS_Primitive
{

	public:
		ACS_CS_MODD_Change();

		ACS_CS_MODD_Change(const ACS_CS_MODD_Change &right);

		virtual ~ACS_CS_MODD_Change();

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

		virtual int setBuffer (const char *buffer, int size);

		virtual int getBuffer (char *buffer, int size) const;

		virtual unsigned short getLength () const;

		virtual ACS_CS_Primitive * clone () const;

		std::string getBootImage() const {return this->bootImage;}

		uint16_t getTimeout() const {return this->timeout;}

		int getMacAddress(char * mac, int size) const;

		void setBootImage(std::string bootImage) {this->bootImage = bootImage;}

		void setTimeout(uint16_t timeout) {this->timeout = timeout;}

		void setMacAddress(char * mac, int size);

	private:
		ACS_CS_MODD_Change & operator=(const ACS_CS_MODD_Change &right);

		std::string bootImage;

		uint16_t timeout;

		char * macAddress;
};


#endif /* ACS_CS_MODD_CHANGE_H_ */
