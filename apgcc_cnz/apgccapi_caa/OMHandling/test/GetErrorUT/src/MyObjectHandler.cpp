/*
 * MyObjectHandler.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: xpaomaz
 */

#include "MyObjectHandler.h"

//MyObjectHandler::MyObjectHandler() {
//	// TODO Auto-generated constructor stub
//
//}
//
//MyObjectHandler::~MyObjectHandler() {
//	// TODO Auto-generated destructor stub
//}

MyObjectHandler::MyObjectHandler(){

}

ACS_CC_ReturnType MyObjectHandler::updateCallback(const char* p_objName, const char* p_attrName){

	printf("------------------------------------------------------------\n");
	printf("                   updateCallbak called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;
	cout<<"Attribute Name: "<<p_attrName<<endl;


	printf("------------------------------------------------------------\n");
	return ACS_CC_SUCCESS;

}
