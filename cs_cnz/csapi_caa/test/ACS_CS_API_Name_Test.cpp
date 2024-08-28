/*
 * ACS_CS_API_Name_Test.cpp
 *
 *  Created on: Sep 1, 2010
 *      Author: mann
 */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "ACS_CS_API_Name_Test.h"

#include "ACS_CS_API.h"

using std::string;
using std::cout;

//ACS_CS_API_Name * testObject = new ACS_CS_API_Name_Implementation();

void copyConstructorTest() {

	// String with name
	char * s1 = "One string";

	// Create two name objects, the second with the copy constructor
	ACS_CS_API_Name n1(s1);
	ACS_CS_API_Name n2(n1);

	// Compare length of name
	unsigned int nameLength = n2.length();
	ASSERTM("Name length not equal", nameLength == n1.length());

	// Compare the names
	char * s2 = new char[nameLength];
	n2.getName(s2, nameLength);
	ASSERTM("Returned name not equal", strcmp(s1, s2) == 0);
	delete [] s2;
}

void assignmentOperatorTest() {

	// String with name
	char * s1 = "One string";

	// Create two name objects, the second with the copy constructor
	ACS_CS_API_Name n1(s1);
	ACS_CS_API_Name n2 = n1;

	// Compare length of name
	unsigned int nameLength = n2.length();
	ASSERTM("Name length not equal", nameLength == n1.length());

	// Compare the names
	char * s2 = new char[nameLength];
	n2.getName(s2, nameLength);
	ASSERTM("Returned name not equal", strcmp(s1, s2) == 0);
	delete [] s2;
}

void setNameTest() {

	// String with name
	char * s1 = "One string";
	char * s2 = "Another string";

	// Create name object
	ACS_CS_API_Name n1(s1);

	// Compare the name
	unsigned int nameLength = n1.length();
	char * s3 = new char[nameLength];
	n1.getName(s3, nameLength);
	ASSERTM("Returned name not equal", strcmp(s1, s3) == 0);
	delete [] s3;

	// Set new name
	n1.setName(s2);

	// Compare new name
	nameLength = n1.length();
	s3 = new char[nameLength];
	n1.getName(s3, nameLength);
	ASSERTM("Returned name not equal", strcmp(s2, s3) == 0);
	delete [] s3;
}


int main(int argc, char * argv[]) {

	cute::suite s;

	s.push_back(CUTE(copyConstructorTest));
	s.push_back(CUTE(assignmentOperatorTest));
	s.push_back(CUTE(setNameTest));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[CS API Cute Test Suite]");



	return 0;
}
