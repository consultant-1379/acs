/*
 * ACS_TRAPDS_StructVariable.cpp
 *
 *  Created on: 16/gen/2012
 *      Author: renato
 */

#include "ACS_TRAPDS_StructVariable.h"

ACS_TRAPDS_StructVariable::~ACS_TRAPDS_StructVariable()
{
	// TODO Auto-generated destructor stub
}


ACS_TRAPDS_StructVariable::ACS_TRAPDS_StructVariable(struct snmp_pdu *pdu, std::string ipTrap)
{
	this->pdu=pdu;
	this->ipTrap=ipTrap;
}



