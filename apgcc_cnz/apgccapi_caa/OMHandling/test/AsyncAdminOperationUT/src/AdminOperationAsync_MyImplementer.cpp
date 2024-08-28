/*
 * AdminOperation_MyImplementer.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#include "AdminOperationAsync_MyImplementer.h"

AdminOperationAsync_MyImplementer::AdminOperationAsync_MyImplementer() {
	// TODO Auto-generated constructor stub

}

void AdminOperationAsync_MyImplementer::objectManagerAdminOperationCallback(
		ACS_APGCC_InvocationType invocation,
		int returnVal ,
		int error
		) {

	printf("------------------------------------------------------------\n");
	printf("                  objectManagerAdminOperationCallback             \n");
	printf("------------------------------------------------------------\n");

	cout<<"invocation : "<< invocation <<endl;
	cout<<"returnVal  : "<< returnVal <<endl;
	cout<<"error      : "<< error <<endl;
	structCode newVal = { error , returnVal };

	if (error == 1)
		mymap[invocation] = newVal;
	else
		mymap[invocation] = newVal;




	printf("------------------------------------------------------------\n");


}
