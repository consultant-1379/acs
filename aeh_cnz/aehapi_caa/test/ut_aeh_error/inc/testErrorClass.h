/*
 * testErrorClass.h
 *
 *  Created on: May 20, 2011
 *      Author: xfabron
 */

#ifndef TESTERRORCLASS_H_
#define TESTERRORCLASS_H_


#include "acs_aeh_error.h"

class testErrorClass : public virtual acs_aeh_error
{
public:
	testErrorClass();

	testErrorClass(const testErrorClass& error);

	virtual ~testErrorClass();

	void mySetError(const ACS_AEH_ErrorType errorType);

	void mySetTextError(const char errorMessage[]);

	void mySetTextError(const char prefix[], const char errorMessage[]);

	void mySetTextError(const char prefix[], const char errorMessage[], const char suffix[]);

	void mySetTextError(const char prefix[], const char errorMessage[], const long suffix);

};


#endif /* TESTERRORCLASS_H_ */
