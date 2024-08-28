/*
 * Objecthandler.cpp
 *
 *  Created on: Sep 29, 2010
 *      Author: xfabron
 */

#include "runtimeObjectHandler.h"

RuntimeObjectHandler::RuntimeObjectHandler(){

}

ACS_CC_ReturnType RuntimeObjectHandler::updateCallback(const char* p_objName, const char* p_attrName){

	printf("\n------------------------------------------------------------\n");
	printf("                   updateCallbak called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;
	//cout<<"Attribute Name: "<<p_attrName<<endl;


	printf("------------------------------------------------------------\n");
	return ACS_CC_SUCCESS;

}
