#ifndef HEADER_GUARD_CLASS__acs_apbm_trapmessage
#define HEADER_GUARD_CLASS__acs_apbm_trapmessage acs_apbm_trapmessage

/** @file acs_apbm_trapmessage.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-06-28
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-06-28 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 *	| R-002 | 2011-11-17 | xgiufer      | Second revision.    				  |
 *	+=======+============+==============+=====================================+
 */

#include <vector>

#include "acs_apbm_types.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_trapmessage

//Forward declaration
class acs_apbm_trapmessage_imp;

/** @class acs_apbm_trapmessage acs_apbm_trapmessage.h
 *	@brief acs_apbm_trapmessage class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-06-28
 *
 *	acs_apbm_trapmessage <PUT CLASS DETAILED DESCRIPTION>
 */
class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
public:
	enum TrapTypeConstants {
		SENSOR_STATE_CHANGE	=	1,
		SEL_ENTRY						=	2,
		BOARD_PRESENCE			=	3,
		NIC									=	4,
		RAID								=	5,
		APBM_READY					=	6,
		DISKCONN						=	7
	};

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_trapmessage Default constructor
	 */
	__CLASS_NAME__ ();

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_trapmessage Destructor
	*/
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:

	//=================//
	// Fields Accessor //
	//=================//
public:

	void set_message (const char *msg, unsigned int length);
    void set_OID (int value);
    void set_values (std::vector<int> value);

	int OID () const;
	const std::vector<int> & values () const;
	const char * message () const;
	unsigned message_length () const;

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	acs_apbm_trapmessage_imp * _trapmessage_imp;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_trapmessage




/*

#ifndef ACS_APBM_TrapMsg_h
#define ACS_APBM_TrapMsg_h 1

#include <list>
#include <string>

using namespace std;

class APBM_Export ACS_APBM_TrapMsg
{
  //## begin ACS_APBM_TrapMsg%44E3FEF603B9.initialDeclarations preserve=yes
	friend class ACS_APBM_apiInternal;
	friend class ACS_APBM_MCF;
	friend class ACS_APBM_NICMonitor;
	friend class ACS_APBM_RAIDMonitor;
	friend class ACS_APBM_ClientSession;
   friend class ACS_APBM_CommandHandler;
   friend class ACS_APBM_MagInfo;
   friend class ACS_APBM_BladeManager;
  //## end ACS_APBM_TrapMsg%44E3FEF603B9.initialDeclarations

  public:
      ACS_APBM_TrapMsg();

      ~ACS_APBM_TrapMsg();

  private:

      void set_message (const char *msg, unsigned int length);

      void set_OID ( int value);

      void set_values (list< int> value);


};

inline void ACS_APBM_TrapMsg::set_OID ( int value)
{
  //## begin ACS_APBM_TrapMsg::set_OID%45237198036B.set preserve=no
  OID = value;
  //## end ACS_APBM_TrapMsg::set_OID%45237198036B.set
}

inline void ACS_APBM_TrapMsg::set_values (list< int> value)
{
  //## begin ACS_APBM_TrapMsg::set_values%452371A00138.set preserve=no
  values = value;
  //## end ACS_APBM_TrapMsg::set_values%452371A00138.set
}

//## begin module%452FA51C0196.epilog preserve=yes
//## end module%452FA51C0196.epilog


#endif

*/
