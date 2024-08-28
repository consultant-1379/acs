#include "acs_apbm_trapmessage_imp.h"
#include "acs_apbm_trapmessage.h"

__CLASS_NAME__::__CLASS_NAME__ () : _trapmessage_imp(new acs_apbm_trapmessage_imp()) {}

__CLASS_NAME__::__CLASS_NAME__ (const __CLASS_NAME__ & /*rhs*/) : _trapmessage_imp(0) {}

__CLASS_NAME__::~__CLASS_NAME__ () { delete _trapmessage_imp; }

int __CLASS_NAME__::OID () const {
	return _trapmessage_imp->OID();
}

const std::vector<int> & __CLASS_NAME__::values () const {
	return _trapmessage_imp->values();
}

const char * __CLASS_NAME__::message () const {
	return _trapmessage_imp->message();
}

unsigned __CLASS_NAME__::message_length () const {
	return _trapmessage_imp->message_length();
}

__CLASS_NAME__ & __CLASS_NAME__::operator=(const __CLASS_NAME__ & /*rhs*/) { return *this; }


void __CLASS_NAME__::set_message (const char *msg, unsigned int length){
	_trapmessage_imp->set_message(msg,length);
}

void __CLASS_NAME__::set_OID ( int oid){
	_trapmessage_imp->set_OID(oid);
}

void __CLASS_NAME__::set_values (std::vector<int> value) {
	_trapmessage_imp->set_values(value);
}



/*

//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%44E400F902BF.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%44E400F902BF.cm

//## begin module%44E400F902BF.cp preserve=no
//## end module%44E400F902BF.cp

//## Module: ACS_APBM_TrapMsg%44E400F902BF; Package body
//## Subsystem: ACS_APBM::apbmapi_caa::src%44923CAC001F
//## Source file: Z:\ntacs\apbm_cnz\apbmapi_caa\src\ACS_APBM_TrapMsg.cpp

//## begin module%44E400F902BF.additionalIncludes preserve=no
//## end module%44E400F902BF.additionalIncludes

//## begin module%44E400F902BF.includes preserve=yes
//## end module%44E400F902BF.includes

// ACS_APBM_TrapMsg
#include "ACS_APBM_TrapMsg.h"
//## begin module%44E400F902BF.declarations preserve=no
//## end module%44E400F902BF.declarations

//## begin module%44E400F902BF.additionalDeclarations preserve=yes
//## end module%44E400F902BF.additionalDeclarations


// Class ACS_APBM_TrapMsg

ACS_APBM_TrapMsg::ACS_APBM_TrapMsg()
  //## begin ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.hasinit preserve=no
  //## end ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.hasinit
  //## begin ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.initialization preserve=yes
  //## end ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.initialization
{
  //## begin ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.body preserve=yes
	OID = 0;
	values.clear();
	message = NULL;
	messagelength = 0;
  //## end ACS_APBM_TrapMsg::ACS_APBM_TrapMsg%44E3FEF603B9_const.body
}


ACS_APBM_TrapMsg::~ACS_APBM_TrapMsg()
{
  //## begin ACS_APBM_TrapMsg::~ACS_APBM_TrapMsg%44E3FEF603B9_dest.body preserve=yes
	if(message)
	{
		delete [] message;
	}
  //## end ACS_APBM_TrapMsg::~ACS_APBM_TrapMsg%44E3FEF603B9_dest.body
}



//## Other Operations (implementation)
const int   ACS_APBM_TrapMsg::get_OID ()
{
  //## begin ACS_APBM_TrapMsg::get_OID%4536149703B9.body preserve=yes
	return OID;
  //## end ACS_APBM_TrapMsg::get_OID%4536149703B9.body
}

const list<int>  ACS_APBM_TrapMsg::get_values ()
{
  //## begin ACS_APBM_TrapMsg::get_values%453614C0005D.body preserve=yes
	return values;
  //## end ACS_APBM_TrapMsg::get_values%453614C0005D.body
}

const char  * ACS_APBM_TrapMsg::get_message ()
{
  //## begin ACS_APBM_TrapMsg::get_message%453614E50148.body preserve=yes
	return message;
  //## end ACS_APBM_TrapMsg::get_message%453614E50148.body
}

const unsigned int   ACS_APBM_TrapMsg::get_messagelength ()
{
  //## begin ACS_APBM_TrapMsg::get_messagelength%4536150C006D.body preserve=yes
	return messagelength;
  //## end ACS_APBM_TrapMsg::get_messagelength%4536150C006D.body
}

void ACS_APBM_TrapMsg::set_message (const char *msg, unsigned int length)
{
  //## begin ACS_APBM_TrapMsg::set_message%4576CCE7034B.body preserve=yes
  if(message)
	{
		delete message;
	}
	message = new char[length];
   memcpy(message,msg,length);
   messagelength = length;
  //## end ACS_APBM_TrapMsg::set_message%4576CCE7034B.body
}

// Additional Declarations
  //## begin ACS_APBM_TrapMsg%44E3FEF603B9.declarations preserve=yes
  //## end ACS_APBM_TrapMsg%44E3FEF603B9.declarations

//## begin module%44E400F902BF.epilog preserve=yes
//## end module%44E400F902BF.epilog

*/
