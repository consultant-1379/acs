/*
 * testErrorClass.cpp
 *
 *  Created on: May 20, 2011
 *      Author: xfabron
 */


#include "testErrorClass.h"

testErrorClass::testErrorClass(){};

testErrorClass::testErrorClass(const testErrorClass& error):acs_aeh_error(error){ }

testErrorClass::~testErrorClass(){};

void testErrorClass::mySetError(const ACS_AEH_ErrorType errorType){

	this->setError(errorType);

}


void testErrorClass::mySetTextError(const char errorMessage[]){

	setErrorText(errorMessage);
}


void testErrorClass::mySetTextError(const char prefix[], const char errorMessage[]){

	setErrorText(prefix, errorMessage);

}


void testErrorClass::mySetTextError(const char prefix[], const char errorMessage[], const char suffix[]){

	setErrorText(prefix, errorMessage, suffix);
}


void testErrorClass::mySetTextError(const char prefix[], const char errorMessage[], const long suffix){

	setErrorText(prefix, errorMessage, suffix);
}
