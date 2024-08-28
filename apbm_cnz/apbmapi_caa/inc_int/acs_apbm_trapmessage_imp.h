#ifndef HEADER_GUARD_CLASS__acs_apbm_trapmessage_imp
#define HEADER_GUARD_CLASS__acs_apbm_trapmessage_imp acs_apbm_trapmessage_imp

#include <vector>

#include "acs_apbm_types.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_trapmessage_imp

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ ();

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:
	int get (acs_apbm::trap_handle_t trap_handle);

	//=================//
	// Fields Accessor //
	//=================//
public:

	void set_message (const char *msg, unsigned int length);
    void set_OID ( int value);
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
	int _OID;
	std::vector<int> _values;
	char * _message;
	unsigned _message_length;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_trapmessage_imp
