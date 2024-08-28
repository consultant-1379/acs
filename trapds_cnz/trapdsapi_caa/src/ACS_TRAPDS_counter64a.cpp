/*
 * ACS_TRAPDS_counter64a.cpp
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */


#include "ACS_TRAPDS_counter64a.h"

ACS_TRAPDS_counter64a::ACS_TRAPDS_counter64a() {
	// TODO Auto-generated constructor stub

}

ACS_TRAPDS_counter64a::~ACS_TRAPDS_counter64a() {
	// TODO Auto-generated destructor stub
}


ACS_TRAPDS_counter64a::ACS_TRAPDS_counter64a(u_long high,u_long low)
{
	this->high=high;
	this->low=low;

}
