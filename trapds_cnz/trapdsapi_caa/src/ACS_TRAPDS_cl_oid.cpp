/*
 * ACS_TRAPDS_cl_oid.cpp
 *
 *  Created on: 17/gen/2012
 *      Author: renato
 */

#include "ACS_TRAPDS_cl_oid.h"

ACS_TRAPDS_cl_oid::ACS_TRAPDS_cl_oid() {

}


ACS_TRAPDS_cl_oid::ACS_TRAPDS_cl_oid(int lun,oid *o) {


	this->length=lun;

	for(int t=0;t<150;t++)
		oids[t]=0;

	for(int t=0;t<length;t++)
		oids[t]=o[t];
	// TODO Auto-generated constructor stub

}

void ACS_TRAPDS_cl_oid::setData(int lun,oid * o)
{
	int t=0;

	for(t=0;t<lun;t++)
	{
		oids[t]=o[t];
	}


	this->length=lun;

}

ACS_TRAPDS_cl_oid::~ACS_TRAPDS_cl_oid() {
	// TODO Auto-generated destructor stub
}
