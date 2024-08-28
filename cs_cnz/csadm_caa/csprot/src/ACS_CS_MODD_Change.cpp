/*
 * ACS_CS_MODD_Change.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */


#include "ACS_CS_MODD_Change.h"

#include <string.h>

#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_MODD_Change_TRACE);


ACS_CS_MODD_Change::ACS_CS_MODD_Change():
	ACS_CS_Primitive(),
	timeout(0),
	macAddress(0)
{}


ACS_CS_MODD_Change::ACS_CS_MODD_Change(const ACS_CS_MODD_Change &right):
		ACS_CS_Primitive(),
		bootImage(right.bootImage),
		timeout(right.timeout),
		macAddress(0)
{
	if (right.macAddress)
	{
		this->macAddress = new char[6];
		memcpy(this->macAddress, right.macAddress, 6);
	}
}


ACS_CS_MODD_Change::~ACS_CS_MODD_Change()
{
	if (macAddress)
		delete [] macAddress;
}


ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_MODD_Change::getPrimitiveType () const
{
	return ACS_CS_Protocol::Primitive_MODD_Change;
}


int ACS_CS_MODD_Change::setBuffer (const char *buffer, int size)
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Change_TRACE,
			"(%t) ACS_CS_MODD_Change::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 12 )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_MODD_Change_TRACE,
			"(%t) ACS_CS_MODD_Change::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}


	// Get mac address
	if (this->macAddress)
	{
		delete [] this->macAddress;
		this->macAddress = 0;
	}

	this->macAddress = new char[6];
	memcpy(this->macAddress, buffer, 6);

	// Get timeout
	this->timeout = *( reinterpret_cast<const uint16_t *>(buffer + 6) );

	// Get name length
	uint16_t nameLength = *( reinterpret_cast<const uint16_t *>(buffer + 8) );

	// Get boot image name
	if (nameLength == (size - 10) )
	{
		char * name = new char[nameLength];
		memcpy(name, buffer + 10, nameLength);
		name[nameLength - 1] = 0;
		this->bootImage = name;
		delete [] name;
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODD_Change_TRACE,
			"(%t) ACS_CS_MODD_Change::setBuffer()\n"
			"Error: Invalid name length, length = %d\n",
			nameLength ));

		return -1;
	}

	return 0;
}


int ACS_CS_MODD_Change::getBuffer (char *buffer, int size) const
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Change_TRACE,
			"(%t) ACS_CS_MODD_Change::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_MODD_Change_TRACE,
			"(%t) ACS_CS_MODD_Change::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}


	// Copy mac address
	if (this->macAddress)
	{
		memcpy(buffer, this->macAddress, 6);
	}

	// Copy timeout
	uint16_t tempShort = this->timeout;
	memcpy(buffer + 6, reinterpret_cast<char *>(&tempShort), sizeof(tempShort));

	// Copy namelength
	tempShort = this->bootImage.length() + 1;
	memcpy(buffer + 8, reinterpret_cast<char *>(&tempShort), sizeof(tempShort));

	// Copy boot image name
	memcpy(buffer + 10, this->bootImage.c_str(), this->bootImage.length());
	buffer[10 + this->bootImage.length()] = 0;

	return 0;
}


unsigned short ACS_CS_MODD_Change::getLength () const
{
	return this->bootImage.length() + 11;
}


ACS_CS_Primitive * ACS_CS_MODD_Change::clone () const
{
	return new ACS_CS_MODD_Change(*this);
}



int ACS_CS_MODD_Change::getMacAddress(char * mac, int size) const
{
	if (mac == 0)						// Check for valid memory
		return -1;
	else if (size != 6)	// Check for valid size
		return -1;
	else if (this->macAddress == 0)		// Check for valid source
		return -1;

	memcpy(mac, this->macAddress, 6);	// Copy MAC

	return 0;
}


void ACS_CS_MODD_Change::setMacAddress(char * mac, int size)
{
	if (mac == 0)			// Check for valid memory
		return;
	else if (size != 6)		// Check for valid size
		return;

	if (this->macAddress)	// Delete old MAC
	{
		delete [] this->macAddress;
		this->macAddress = 0;
	}

	// Copy new MAC
	this->macAddress = new char[6];

	if (this->macAddress)
		memcpy(this->macAddress, mac, 6);
}

