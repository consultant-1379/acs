//********************************************************************************
//
// NAME
// acs_rtr_gohtransmitinfo.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-17 by XSAMECH
//
// DESCRIPTION 
// This class packages a file or a block used in the output queue.
//
//********************************************************************************

#include "acs_rtr_gohtransmitinfo.h"
#include "acs_rtr_global.h"
#include "acs_rtr_manager.h"

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
ACS_RTR_GohTransmitInfo::ACS_RTR_GohTransmitInfo(void) : _file(0), _block(0), _stat(0), _cpname(0)
{
	_cpname = new string(DEFAULT_CPNAME);		//for TR HL13904
	_cpId = DEFAULT_CPID;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
ACS_RTR_GohTransmitInfo::~ACS_RTR_GohTransmitInfo(void) {
	//for TR HL13904
	try 
	{ 
		if (_file) 
		{
			delete _file;
			_file = 0;
		}
	} catch (...) {}
	try 
	{ 
		if (_block) 
		{
			delete _block; 
			_block = 0;
		}
	} catch (...) {}
	try 
	{ 
		if (_cpname) 
		{
			delete _cpname; 
            _cpname = 0;
		}
	} catch (...){}
	//the owner of stat is ACS_RTR_Manager in single cp system,
	// or it's ACS_RTR_ACAChannel in multiple cp system
}

//---------------------------------------------------------------------------
// set rtr file
//---------------------------------------------------------------------------
void ACS_RTR_GohTransmitInfo::setRTRFile(RTRfile* file) {
	_file = file;
}

//---------------------------------------------------------------------------
// set rtr block
//---------------------------------------------------------------------------
void ACS_RTR_GohTransmitInfo::setRTRBlock(RTRblock* block) {
	_block = block;
}

//---------------------------------------------------------------------------
// set statistics file
//---------------------------------------------------------------------------
void ACS_RTR_GohTransmitInfo::setStatistics(RTR_statistics* stat) {
	_stat = stat;
}

//---------------------------------------------------------------------------
// set cp name
//---------------------------------------------------------------------------
void ACS_RTR_GohTransmitInfo::setCpName(string& cpname) {
	_cpname->assign(cpname);
}

//---------------------------------------------------------------------------
// set cp id
//---------------------------------------------------------------------------
void ACS_RTR_GohTransmitInfo::setCpId(short cpId) {
	_cpId = cpId;
}
