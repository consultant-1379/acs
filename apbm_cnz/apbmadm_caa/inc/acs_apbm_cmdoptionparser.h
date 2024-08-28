#ifndef HEADER_GUARD_CLASS__acs_apbm_cmdoptionparser
#define HEADER_GUARD_CLASS__acs_apbm_cmdoptionparser acs_apbm_cmdoptionparser

/** @file acs_apbm_cmdoptionparser.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-12
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
 *	| R-001 | 2011-07-12 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <unistd.h>
#include <getopt.h>

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_cmdoptionparser

/** @class acs_apbm_cmdoptionparser acs_apbm_cmdoptionparser.h
 *	@brief acs_apbm_cmdoptionparser class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-12
 *
 *	acs_apbm_cmdoptionparser <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_cmdoptionparser constructor
	 */
	inline __CLASS_NAME__ (int argc, char * const argv []) : _argc(argc), _argv(argv), _noha(0), _cba(-1) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_cmdoptionparser Destructor
	 */
	inline ~__CLASS_NAME__ () {}

	//=================//
	// Field Accessors //
	//=================//
public:
	inline int argc () const { return _argc; }

	inline const char * const * argv () const { return _argv; }

	inline int noha () const { return _noha; }
	inline int cba () const { return _cba; }
	const char * program_name () const;

	//===========//
	// Functions //
	//===========//
public:
	int parse ();

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	int _argc;
	char * const * _argv;
	int _noha;
	int _cba;

	static const char * _program_name;
	static struct option _long_options [];
};

#endif // HEADER_GUARD_CLASS__acs_apbm_cmdoptionparser
