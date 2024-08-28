//============================================================================
// Name        : envImmTest_Destroy.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "OmHandler.h"
using namespace std;

int main(int argc, char * argv[]) {

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDelete = const_cast<char*>("Test_config");

	int c = 0;

	while( (c = getopt(argc, argv, "c:")) != -1){
		switch (c) {
			case 'c':

				nameClassToBeDelete = optarg;

				break;

			case '?':
				if( argc > 3)
					ACE_OS::fprintf(stdout, "error - Too many arguments \n");

				else if( optopt == 'c' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( isprint(optopt) )
					ACE_OS::fprintf(stdout, "error - Unrecognized option: -%c\n", optopt);

				return -1;
				break;
			default:
				break;
		}//END switch(c)

	}

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	result = immHandler.removeClass(nameClassToBeDelete);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: removeClass FAILURE!!!\n";
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDelete<<" deleted.\n";
	}

	return 0;
}
