/*
 * ACS_CS_MODD_RevertResponse.h
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */

#ifndef ACS_CS_MODD_REVERTRESPONSE_H_
#define ACS_CS_MODD_REVERTRESPONSE_H_


#include "ACS_CS_Primitive.h"

#include <string>

#include "ACS_CS_Protocol.h"



class ACS_CS_MODD_RevertResponse: public ACS_CS_Primitive
{

	public:
		ACS_CS_MODD_RevertResponse();

		ACS_CS_MODD_RevertResponse(const ACS_CS_MODD_RevertResponse &right);

		virtual ~ACS_CS_MODD_RevertResponse();

		virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

		virtual int setBuffer (const char *buffer, int size);

		virtual int getBuffer (char *buffer, int size) const;

		virtual unsigned short getLength () const;

		virtual ACS_CS_Primitive * clone () const;

		std::string getDescription() const {return this->description;}

		ACS_CS_Protocol::MODD_Result_Code getResultCode() const {return this->resultCode;}

		void setDescription(std::string description) {this->description = description;}

		void setResultCode(ACS_CS_Protocol::MODD_Result_Code resultCode) {this->resultCode = resultCode;}

	private:
		ACS_CS_MODD_RevertResponse & operator=(const ACS_CS_MODD_RevertResponse &right);

		ACS_CS_Protocol::MODD_Result_Code resultCode;

		std::string description;

};


#endif /* ACS_CS_MODD_REVERTRESPONSE_H_ */
