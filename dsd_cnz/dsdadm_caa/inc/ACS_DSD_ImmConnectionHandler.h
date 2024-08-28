
#ifndef HEADER_GUARD_CLASS__ACS_DSD_ImmConnectionHandler 
#define HEADER_GUARD_CLASS__ACS_DSD_ImmConnectionHandler ACS_DSD_ImmConnectionHandler

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ImmConnectionHandler

/** @class ACS_DSD_ImmConnectionHandler ACS_DSD_ImmConnectionHandler.h
 *	@brief ACS_DSD_ImmConnectionHandler class
 *	@author 
 *	@date 
 *	@version 
 *
 *	ACS_DSD_ImmConnectionHandler Class detailed description
 */

#include "ACS_APGCC_RuntimeOwner.h"
#include "ACS_CC_Types.h"

class __CLASS_NAME__ : public ACS_APGCC_RuntimeOwner {

public:
	enum ImmConnState {BAD, GOOD};

	inline ACS_DSD_ImmConnectionHandler():_immConnState(GOOD){}
	inline ~ACS_DSD_ImmConnectionHandler(){}

	/**Implementation of virtual method **/
	inline ACS_CC_ReturnType updateCallback(const char* /*p_objName*/, const char* /*p_attrName*/){ return (ACS_CC_ReturnType)0;}

	/// Indicates if the connection to IMM is good (i.e not corrupted)
	inline bool good() { return (_immConnState == GOOD); }

	/// Set the state of IMM connection
	inline void setImmConnState(ImmConnState newConnState) {_immConnState = newConnState; }

	/// Takes an APGCC library error code, and returns true if and only if it is a critical error, for which a recovery action is needed
	static bool is_critical_error( int apgcc_err_code);

private:
	ImmConnState _immConnState;		/// state of IMM connection
};

#endif /* HEADER_GUARD_CLASS__ACS_DSD_ImmConnectionHandler */
