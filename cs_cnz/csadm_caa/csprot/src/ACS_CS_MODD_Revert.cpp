/*
 * ACS_CS_MODD_Revert.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */


#include "ACS_CS_MODD_Revert.h"


#include <string.h>

#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_MODD_Revert_TRACE);


ACS_CS_MODD_Revert::ACS_CS_MODD_Revert():
	ACS_CS_Primitive(),
	macAddress(0)
{}


ACS_CS_MODD_Revert::ACS_CS_MODD_Revert(const ACS_CS_MODD_Revert &right):
		ACS_CS_Primitive(),
		macAddress(0)
{
	if (right.macAddress)
	{
		this->macAddress = new char[6];
		memcpy(this->macAddress, right.macAddress, 6);
	}
}


ACS_CS_MODD_Revert::~ACS_CS_MODD_Revert()
{
	if (macAddress)
		delete [] macAddress;
}


ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_MODD_Revert::getPrimitiveType () const
{
	return ACS_CS_Protocol::Primitive_MODD_Revert;
}


int ACS_CS_MODD_Revert::setBuffer (const char *buffer, int size)
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Revert_TRACE,
			"(%t) ACS_CS_MODD_Revert::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size != 6 )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_MODD_Revert_TRACE,
			"(%t) ACS_CS_MODD_Revert::setBuffer()\n"
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

	return 0;
}


int ACS_CS_MODD_Revert::getBuffer (char *buffer, int size) const
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Revert_TRACE,
			"(%t) ACS_CS_MODD_Revert::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != 6)	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_MODD_Revert_TRACE,
			"(%t) ACS_CS_MODD_Revert::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}


	// Copy mac address
	if (this->macAddress)
	{
		memcpy(buffer, this->macAddress, 6);
	}

	return 0;
}


unsigned short ACS_CS_MODD_Revert::getLength () const
{
	return 6;
}


ACS_CS_Primitive * ACS_CS_MODD_Revert::clone () const
{
	return new ACS_CS_MODD_Revert(*this);
}



int ACS_CS_MODD_Revert::getMacAddress(char * mac, int size) const
{
	if (mac == 0)						// Check for valid memory
		return -1;
	else if (size != 6)	// Check for valid size
		return -1;
	else if (this->macAddress == 0)		// Check for valid source
		return-1;

	memcpy(mac, this->macAddress, 6);	// Copy MAC

	return 0;
}


void ACS_CS_MODD_Revert::setMacAddress(char * mac, int size)
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

